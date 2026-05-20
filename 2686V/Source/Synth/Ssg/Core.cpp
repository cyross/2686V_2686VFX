#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"

const std::array<float, 9> SsgCore::dutyPresets = { 0.5f, 0.4375f, 0.375f, 0.3125f, 0.25f, 0.20f, 0.1875f, 0.125f, 0.0625f };

namespace {
    // =================================================================
    // 波形ストラテジー配列の定義
    // (引数: ラジアン位相 p, 正規化位相 n, サイン波 s)
    // =================================================================
    using HwEnvGainCalculator = float(*)(float p, float n, bool e);

    const std::array<HwEnvGainCalculator, 8> hwGainStrategies = { {
        [](float p, float n, bool e) { return 1.0f - n; },
        [](float p, float n, bool e) { return (p < 1.0) ? (1.0f - n) : 0.0f; },
        [](float p, float n, bool e) { return e ? (1.0f - n) : n; },
        [](float p, float n, bool e) { return (p < 1.0) ? (1.0f - n) : 0.0f; },
        [](float p, float n, bool e) { return n; },
        [](float p, float n, bool e) { return (p < 1.0) ? n : 1.0f; },
        [](float p, float n, bool e) { return e ? n : (1.0f - n); },
        [](float p, float n, bool e) { return (p < 1.0) ? n : 1.0f; },
    } };
}

SsgCore::SsgCore() : SynthCore() {
}

void SsgCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) {
        m_sampleRate = sampleRate;
    }

    m_targetRate = getTargetRate(m_rateIndex);

    m_noiseGen.prepare(m_targetRate);
    m_adsr.prepare(m_targetRate);
	m_pitchAdsr.prepare(m_targetRate);
	m_ssgSwEnv.prepare(m_targetRate);
    m_lfo.prepare(m_targetRate);

    updatePhaseDelta();
}

void SsgCore::setSampleRate(double sampleRate) {
    m_sampleRate = sampleRate;
}

void SsgCore::setParameters(const SynthParams& params)
{
    m_level = params.ssg.level;
    m_mix = params.ssg.mix;

    m_adsr.setParameters(params.ssg.adsr);
	m_pitchAdsr.setParameters(params.ssg.pitchAdsr);
    m_detune.setParameters(params.ssg.detune, params.ssg.detune2, 1);
	m_ssgSwEnv.setParameters(params.ssg.ssgSwEnv);
    m_lfo.setParameters(
        params.ssg.lfoSyncDelay,
        params.ssg.lfoPmEnable, params.ssg.lfoAmEnable,
        params.ssg.lfoPmFreq, params.ssg.lfoAmFreq,
        params.ssg.lfoPmWave, params.ssg.lfoAmWave,
        params.ssg.lfoPms, params.ssg.lfoPmd,
        params.ssg.lfoAms, params.ssg.lfoAmd,
        params.ssg.lfoAmSmRt
    );

    m_waveform = params.ssg.waveform;

    m_useHwEnv = params.ssg.useHwEnv;
    m_envShape = params.ssg.envShape;
    m_envFreq = params.ssg.envPeriod;

    m_dutyMode = params.ssg.dutyMode;
    m_dutyPreset = params.ssg.dutyPreset;
    m_dutyVar = params.ssg.dutyVar;
    m_dutyInvert = params.ssg.dutyInvert;

    m_triKeyTrack = params.ssg.triKeyTrack;
    m_triPeak = params.ssg.triPeak;
    m_triFreq = params.ssg.triFreq;

    m_noiseGen.setParameters(params.ssg.noiseLevel, params.ssg.noiseFreq, params.ssg.noiseOnNote);

    if (m_rateIndex != params.ssg.rateIndex) {
        m_rateIndex = params.ssg.rateIndex;

        m_targetRate = getTargetRate(m_rateIndex);

        m_noiseGen.updateTargetRate(m_targetRate);
        m_adsr.updateSampleRate(m_targetRate);
        m_pitchAdsr.updateSampleRate(m_targetRate);
		m_ssgSwEnv.updateSampleRate(m_targetRate);
        m_lfo.updateTargetSampleRate(m_targetRate);
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    m_quantizeSteps = getTargetBitDepth(params.ssg.bitDepth);

    updatePhaseDelta();
}

void SsgCore::noteOn(float freq, float velocity, int midiNote)
{
    m_currentLevel = velocity;

    // 基本周波数にデチューン成分を加算
    // Save for recalculation
    m_currentFrequency = m_detune.noteOn(freq);
    m_phase = 0.0f;

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    updatePhaseDelta();

    m_hwEnvPhase = 0.0f;

    // Reset Rate Logic
    m_rateAccumulator = 1.0f;
    m_lastSample = 0.0f;

    m_adsr.noteOn();
    m_pitchAdsr.noteOn();
	m_ssgSwEnv.noteOn();
    m_lfo.noteOn();
}

void SsgCore::noteOff()
{
    m_adsr.noteOff();
	m_pitchAdsr.noteOff();
	m_ssgSwEnv.noteOff();
}

bool SsgCore::isPlaying() const { return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying(); }

// ピッチベンド (0 - 16383, Center=8192)
void SsgCore::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void SsgCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void SsgCore::setPitchBendRatio(float ratio)
{ m_pitchBendRatio = ratio; }

// --- SsgCore.cpp : getSample() を丸ごと差し替え ---

float SsgCore::getSample()
{
    if (!isPlaying()) {
		// ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
		// ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        return 0.0f;
    }

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    if (m_adsr.isBypassed() && m_ssgSwEnv.isBypassed())
    {
        // どちらもバイパスの時は完全な矩形波（Gate）動作
        if (m_adsr.isRelease() || m_ssgSwEnv.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv.bypassedReleasedProcess();
            finalEnv = 1.0f;
        }
    }
    else
    {
        // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
        if (!m_adsr.isBypassed()) {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel; // 掛け算
        }
        else {
            if (m_adsr.isRelease()) m_adsr.bypassedReleasedProcess();
        }

        // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
        if (!m_ssgSwEnv.isBypassed()) {
            finalEnv *= m_ssgSwEnv.process(); // 掛け算
        }
        else {
            if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
        }
    }

    // --- Sample Rate Emulation ---
    double stepSize = m_targetRate / m_sampleRate;
    m_rateAccumulator += stepSize;

    // SSG（矩形波）の場合は、線形補間ではなく「平均化(アベレージング)」が正解
    int steps = 0;
    float sumOut = 0.0f;
	float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);

    // Update core logic only when virtual clock ticks
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_lfo.getSample();

        // ==========================================
        // Opzx7 LFO の計算 (AM / PM)
        // ==========================================

        // 1. Amplitude Modulation (AM / 音量)
        float amMultiplier = 1.0f;

        if (m_lfo.amEnable) {
            // depthDb はセットアップ時に計算済みなので、そのままdB減衰に変換
            float attenDb = m_lfo.value.am * m_lfo.depthDb;
            amMultiplier = std::pow(10.0f, -attenDb / 20.0f);
        }

        // 2. Pitch Modulation (PM / 音程)
        float pitchModCents = 0.0f;

        if (m_lfo.pmEnable) {
            // depthCent も計算済みなので、そのままセント値に変換
            pitchModCents += m_lfo.value.pm * m_lfo.depthCent;
        }

        // セントを周波数倍率(レシオ)に変換
        float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);
 
        // ==========================================
        // LFO Calculation (Software Vibrato)
        // ==========================================
        double lfoInc = m_lfoFreq / m_targetRate;
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

        float lfoVal = 0.0f;
        if (m_lfoPhase < 0.25)      lfoVal = (float)(m_lfoPhase * 4.0);
        else if (m_lfoPhase < 0.75) lfoVal = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
        else                        lfoVal = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

        float modDepth = m_modWheel * 0.03f;
        float mwPitchMod = 1.0f + (lfoVal * modDepth);

        // ==========================================
        // 周波数倍率の決定
        // (PitchBend × Opzx7のPM × ModWheelのPM)
        // ==========================================
        float freqMult = m_pitchBendRatio * opzx7PitchMod * mwPitchMod;

        float phaseInc = 0.0f;
        if (m_waveform == 1 && !m_triKeyTrack) {
            phaseInc = (m_triFreq / (float)m_targetRate) * freqMult;
        }
        else {
            phaseInc = newPhaseDelta * freqMult;
        }

        // ==========================================
        // 1. Hardware Envelope Update
        // ==========================================
        float hwEnvDelta = m_envFreq / (float)m_targetRate;
        m_hwEnvPhase += hwEnvDelta;

        // 位相が無限増大して小数の精度が落ちるのを防ぐラップアラウンド
        if (m_hwEnvPhase >= 2.0) {
            if (m_envShape % 2 == 0) m_hwEnvPhase -= 2.0;
            else m_hwEnvPhase = 2.0;
        }

        float hwEnvGain = 1.0f;
        if (m_useHwEnv)
        {
            double p = m_hwEnvPhase;
            bool isEvenCycle = ((int)p % 2 == 0);
            float phaseNorm = (float)(p - std::floor(p));

            hwEnvGain = hwGainStrategies[m_envShape](p, phaseNorm, isEvenCycle);
        }

        // ==========================================
        // 2. Waveform Generation
        // ==========================================
        float toneSample = 0.0f;

        if (m_waveform == 0) // Pulse
        {
            float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

            if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

            // 極端なデューティ比による波形消失を防ぐ最低保証
            float minDuty = phaseInc;
            if (currentDuty < minDuty) currentDuty = minDuty;
            if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

            toneSample = (m_phase < currentDuty) ? 1.0f : -1.0f;

        }
        else // Triangle
        {
            float phaseNorm = m_phase;
            float k = m_triPeak;
            if (k < 0.001f) k = 0.001f;
            if (k > 0.999f) k = 0.999f;

            if (phaseNorm < k) toneSample = -1.0f + 2.0f * (phaseNorm / k);
            else                toneSample = 1.0f - 2.0f * ((phaseNorm - k) / (1.0f - k));
        }

        m_phase += phaseInc;
        if (m_phase >= 1.0f) m_phase -= 1.0f;

        // ==========================================
        // 3. Noise Generator
        // ==========================================
        m_noiseGen.generate();

        // ==========================================
        // 4. Mixing
        // ==========================================
        float toneGain = 1.0f - m_mix;
        float noiseGain = m_mix;
        float rawMixed = (toneSample * m_level * toneGain) + m_noiseGen.generateSample(noiseGain);

        // AM（トレモロ効果）をここで掛け合わせる
        rawMixed *= hwEnvGain * amMultiplier;

        // ==========================================
        // 5. Quantize (Bit Depth)
        // ==========================================
        float finalOut = 0.0f;
        if (m_quantizeSteps > 0.0f) {
            if (rawMixed > 1.0f) rawMixed = 1.0f;
            if (rawMixed < -1.0f) rawMixed = -1.0f;

            // ディザ(dither)を削除し、実機DACと同じ純粋な四捨五入にする(プレビューのゴミ解消)
            float norm = (rawMixed + 1.0f) * 0.5f;
            float quantized = std::round(norm * m_quantizeSteps) / m_quantizeSteps;
            finalOut = (quantized * 2.0f) - 1.0f;
        }
        else {
            finalOut = rawMixed;
        }

        sumOut += finalOut;
        steps++;
    }

    if (steps > 0) {
        m_lastSample = sumOut / (float)steps;
    }

    return m_lastSample * finalEnv * m_currentLevel;
}

void SsgCore::updatePhaseDelta() {
    m_phaseDelta = m_currentFrequency / m_targetRate;
}

void SsgCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample * 0.5; // モノラルなので、ステレオで言うCと同じ計算
    outR[startSample + sampleIdx] += sample * 0.5; // モノラルなので、ステレオで言うCと同じ計算

    isActive = isPlaying();
}
