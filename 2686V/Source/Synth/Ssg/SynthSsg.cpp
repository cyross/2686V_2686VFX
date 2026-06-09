#include "./SynthSsg.h"

#include "../../Core/Synth/SynthHelpers.h"

const std::array<float, 9> SsgCore::dutyPresets = { 0.5f, 0.4375f, 0.375f, 0.3125f, 0.25f, 0.20f, 0.1875f, 0.125f, 0.0625f };

SsgCore::SsgCore() : SynthCore() {
}

void SsgCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) {
        m_sampleRate = sampleRate;
    }

    m_adsr.prepare(m_sampleRate);
	m_pitchAdsr.prepare(0, m_sampleRate);
	m_ssgSwEnv.prepare(0, m_sampleRate);

    m_targetRate = getTargetRate(m_rateIndex);

	double target = getTargetRate(m_rateIndex);

    m_lfo.prepare(target);
    m_noiseGen.prepare(target);

    updatePhaseDelta();
}

void SsgCore::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void SsgCore::setSampleRate(double sampleRate) {
    m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(m_sampleRate);
    m_pitchAdsr.updateSampleRate(m_sampleRate);
    m_ssgSwEnv.updateSampleRate(m_sampleRate);

    updatePhaseDelta();
}

void SsgCore::setParameters(const SynthParams& params)
{
    m_level = params.ssg.level;
    m_mix = params.ssg.mix;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_adsr.setParameters(params.ssg.adsr);
	m_pitchAdsr.setParameters(params.ssg.pitchAdsr);
    m_detune.setParameters(params.ssg.detune, params.ssg.detune2, params.ssg.detune3, params.ssg.multiple, params.ssg.multipleRatio);
	m_ssgSwEnv.setParameters(params.ssg.ssgSwEnv);
    m_lfo.setParameters(
        params.ssg.lfoPmSyncDelay,
        params.ssg.lfoAmSyncDelay,
        params.ssg.lfoPmEnable,
        params.ssg.lfoAmEnable,
        params.ssg.lfoPmFreq,
        params.ssg.lfoAmFreq,
        params.ssg.lfoPmWave,
        params.ssg.lfoAmWave,
        params.ssg.lfoPms,
        params.ssg.lfoPmd,
        params.ssg.lfoAms,
        params.ssg.lfoAmd,
        params.ssg.lfoAmSmRt
    );

    m_fixMode.setParameters(params.ssg.fixedMode, params.ssg.fixedFreq);

    m_waveform = params.ssg.waveform;

    m_useHwEnv = params.ssg.useHwEnv;
    m_envShape = params.ssg.envShape;
    m_envFreq = params.ssg.envPeriod;

    m_dutyMode = params.ssg.dutyMode;
    m_dutyPreset = params.ssg.dutyPreset;
    m_dutyVar = params.ssg.dutyVar;
    m_dutyInvert = params.ssg.dutyInvert;
    m_dutyFc = params.ssg.dutyFc;
    m_dutyFcFluc = params.ssg.dutyFcFluc;

    m_triKeyTrack = params.ssg.triKeyTrack;
    m_triPeak = params.ssg.triPeak;
    m_triFreq = params.ssg.triFreq;

    m_noiseGen.setParameters(params.ssg.noiseLevel, params.ssg.noiseFreq, params.ssg.noiseOnNote);

    if (m_rateIndex != params.ssg.rateIndex) {
        m_rateIndex = params.ssg.rateIndex;

        m_targetRate = getTargetRate(m_rateIndex);

        m_noiseGen.updateTargetRate(m_targetRate);
        m_lfo.updateTargetSampleRate(m_targetRate);
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    m_quantizeSteps = getTargetBitDepth(params.ssg.bitDepth);

    m_pitchResetOnLegato = params.pitchResetOnLegato;

    updatePhaseDelta();
}

void SsgCore::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
    // =====================================================================
    // モノフォニック・レガート時は、音量（ベロシティ）を更新しない！
    // 1音目の音量をそのまま引き継ぐことで、音量ジャンプを完全に防ぐ。
    // =====================================================================
    if (!isLegato) {
        // 新規発音の時だけ、ベロシティからベースレベルを計算する
        m_baseLevel = std::max(0.01f, velocity * 0.25f);
    }
    // (レガート時は m_baseLevel は古い値のまま維持される)

    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = freq;

    // ユニゾン時の位相のズレ(0.0〜1.0)を算出
    m_unisonPhaseOffset = 0.0f;

    if (m_unisonTotal > 1) {
        // 現在のボイスが全体のどこに配置されるかを -1.0(一番下) 〜 1.0(一番上) で算出
        // 例(3ボイス): -1.0,  0.0,  1.0
        // 例(4ボイス): -1.0, -0.33, 0.33, 1.0
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f;

        // 最大デチューン幅に位置を掛け合わせて、このボイスのズレ量(セント)を決定
        float centOffset = spreadPos * (float)m_unisonDetuneAmt;

        // セント値(Cents) を周波数倍率(Ratio)に変換する
        // (1200セント ＝ 1オクターブ ＝ 周波数2倍)
        finalFreq = freq * std::pow(2.0f, centOffset / 1200.0f);

        // ボイスインデックスに応じて位相を均等に散らす (例: 3ボイスなら 0.0, 0.33, 0.66)
        m_unisonPhaseOffset = (float)m_unisonIndex / (float)m_unisonTotal;
    }

    // 基本周波数にデチューン成分を加算
    // Save for recalculation
    finalFreq = m_fixMode.noteOn(finalFreq);
    m_currentFrequency = m_detune.noteOn(finalFreq);

    if (!isLegato) {
        if (!m_isMonoMode) {
            m_phase = (m_unisonPhaseOffset * juce::MathConstants<float>::twoPi);

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= juce::MathConstants<float>::twoPi) {
                m_phase -= juce::MathConstants<float>::twoPi;
            }
            m_lfo.noteOn();
        }

        m_rateAccumulator = 0.0;
        m_hwEnvPhase = 0.0f;
        m_lastSample = 0.0f;
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();
    updatePhaseDelta();

    if (!isLegato) {
        m_currentLevel = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.noteOn();
        }
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }
}

void SsgCore::noteOff()
{
    m_adsr.noteOff();

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }
    if (!m_ssgSwEnv.isBypass()) {
        m_ssgSwEnv.noteOff();
    }
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
    if (!isPlaying() && m_adsr.isBypass()) {
        if (m_pitchAdsr.isBypass()) {
            m_pitchAdsr.bypassedReleasedProcess();
        }

        if (m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.bypassedReleasedProcess();
        }

        return 0.0f;
    }

    if (!isPlaying()) {
		// ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
		// ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        return 0.0f;
    }

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
    if (!m_adsr.isBypass()) {
        m_currentLevel = m_adsr.process(m_currentLevel);
        finalEnv *= m_currentLevel; // 掛け算
    }
    else {
        if (m_adsr.isRelease()) m_adsr.bypassedReleasedProcess();
        else {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel; // 掛け算
        }
    }

    // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
    if (!m_ssgSwEnv.isBypass()) {
        finalEnv *= m_ssgSwEnv.process(); // 掛け算
    }
    else {
        if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
    }

    // --- Sample Rate Emulation ---
    double stepSize = m_targetRate / m_sampleRate;
    m_rateAccumulator += stepSize;

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);

    // Update core logic only when virtual clock ticks
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // 前回のサンプルを保存 (線形補間用)
        m_prevSample = m_lastSample;

        m_lfo.getSample();

        // ==========================================
        // Opzx7 LFO の計算 (AM / PM)
        // ==========================================

        // 1. Amplitude Modulation (AM / 音量)
        float amMultiplier = 1.0f;

        if (m_lfo.am.enable) {
            // depthDb はセットアップ時に計算済みなので、そのままdB減衰に変換
            float attenDb = m_lfo.value.am * m_lfo.am.depthDb;
            amMultiplier = std::pow(10.0f, -attenDb / 20.0f);
        }

        // 2. Pitch Modulation (PM / 音程)
        float pitchModCents = 0.0f;

        if (m_lfo.pm.enable) {
            // depthCent も計算済みなので、そのままセント値に変換
            pitchModCents += m_lfo.value.pm * m_lfo.pm.depthCent;
        }

        // セントを周波数倍率(レシオ)に変換
        float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);
 
        float modDepth = m_modWheel * 0.03f;
        float mwPitchMod = 1.0f + (m_lfo.value.pm * modDepth);

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

            switch (m_envShape) {
            case 0:
                hwEnvGain = 1.0f - phaseNorm;
                break;
            case 1:
                hwEnvGain = (p < 1.0) ? (1.0f - phaseNorm) : 0.0f;
                break;
            case 2:
                hwEnvGain = isEvenCycle ? (1.0f - phaseNorm) : phaseNorm;
                break;
            case 3:
                hwEnvGain = (p < 1.0) ? (1.0f - phaseNorm) : 0.0f;
                break;
            case 4:
                hwEnvGain = phaseNorm;
                break;
            case 5:
                hwEnvGain = (p < 1.0) ? phaseNorm : 1.0f;
                break;
            case 6:
                hwEnvGain = (p < 1.0) ? phaseNorm : 1.0f;
                break;
            case 7:
                hwEnvGain = isEvenCycle ? phaseNorm : (1.0f - phaseNorm);
                break;
            default:
                hwEnvGain = phaseNorm;
            }
        }

        // ==========================================
        // 2. Waveform Generation
        // ==========================================
        float toneSample = 0.0f;

        if (m_waveform == 0) // Pulse
        {
            if (m_dutyFc) {
                float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                // 極端なデューティ比による波形消失を防ぐ最低保証
                float minDuty = phaseInc;
                if (currentDuty < minDuty) currentDuty = minDuty;
                if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

                toneSample = (m_phase < currentDuty)
                    ? (1.0f - m_dutyFcFluc * (m_phase / currentDuty))
                    : (-1.0f + m_dutyFcFluc * ((m_phase - currentDuty) / (1.0f - currentDuty)));
             }
            else {
                float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                // 極端なデューティ比による波形消失を防ぐ最低保証
                float minDuty = phaseInc;
                if (currentDuty < minDuty) currentDuty = minDuty;
                if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

                toneSample = (m_phase < currentDuty) ? 1.0f : -1.0f;
            }
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

        m_lastSample = finalOut;
    }

    // 線形補間を適用して波形を滑らかに出力する
    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    float interpolatedSample = m_prevSample + (m_lastSample - m_prevSample) * fraction;

    return interpolatedSample * finalEnv * m_baseLevel * 8.0f; 
}

void SsgCore::updatePhaseDelta() {
    m_phaseDelta = m_currentFrequency / m_targetRate;
}

void SsgCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

    if (m_unisonTotal > 1) {
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f; // -1.0 to 1.0

        // spreadPosが -1(L) の時、Right側の音量を下げる。逆も然り。
        float panOffset = spreadPos * m_unisonSpreadAmt * 0.5f; // 最大で ±0.5 動く

        basePanL = std::clamp(basePanL - panOffset, 0.0f, 1.0f);
        basePanR = std::clamp(basePanR + panOffset, 0.0f, 1.0f);

        // 音量補正 (ボイス数が増えると爆音になるため下げる)
        // ルートを取るか、単純に割るかは好みですが、単純割りの方が安全です
        float gainComp = 1.0f / std::sqrt((float)m_unisonTotal);
        sample *= gainComp;
    }

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
