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
    m_ssgSwEnv11.prepare(0, m_sampleRate);
    m_ssgSwPenv11.prepare(0, m_sampleRate);

    m_targetRate = getTargetRate(m_rateIndex);

    m_lfo.prepare(m_targetRate);
    m_noiseGen.prepare(m_targetRate);
    m_ssgHwEnv.prepare(m_targetRate);
    m_phaseDelta = m_currentFrequency / m_targetRate;
}

void SsgCore::setSampleRate(double sampleRate) {
    m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(m_sampleRate);
    m_pitchAdsr.updateSampleRate(m_sampleRate);
    m_ssgSwEnv.updateSampleRate(m_sampleRate);
    m_ssgSwEnv11.updateSampleRate(m_sampleRate);
    m_ssgSwPenv11.updateSampleRate(m_sampleRate);
}

void SsgCore::setParameters(const SynthParams& params)
{
    m_level = params.ssg.level;

    m_tone = params.ssg.tn.tone;
    m_mix = params.ssg.tn.mix;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_adsr.setParameters(params.ssg.adsr);
	m_pitchAdsr.setParameters(params.ssg.pitchAdsr);
    m_detune.setParameters(params.ssg.detune);
	m_ssgSwEnv.setParameters(params.ssg.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.ssg.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.ssg.ssgSwPEnv11);
    m_ssgHwEnv.setParameters(params.ssg.env);
    m_wtMod.setParameters(params.ssg.wtMod);
    m_lfo.setParameters(params.ssg.lfo);

    m_fixMode.setParameters(params.ssg.fix);

    m_waveform = params.ssg.waveform;

    m_dutyMode = params.ssg.duty.mode;
    m_dutyPreset = params.ssg.duty.preset;
    m_dutyVar = params.ssg.duty.var;
    m_dutyInvert = params.ssg.duty.invert;
    m_dutyFc = params.ssg.duty.fc;
    m_dutyFcFluc = params.ssg.duty.fcFluc;

    m_triKeyTrack = params.ssg.tri.keyTrack;
    m_triPeak = params.ssg.tri.peak;
    m_triFreq = params.ssg.tri.freq;

    m_noiseGen.setParameters({ .level = params.ssg.tn.noiseLevel, .noiseOnNote = params.ssg.tn.noiseOnNote, .baseFreq = params.ssg.tn.noiseFreq });

    if (m_rateIndex != params.ssg.quality.rate) {
        m_rateIndex = params.ssg.quality.rate;

        m_targetRate = getTargetRate(m_rateIndex);

        m_noiseGen.updateTargetRate(m_targetRate);
        m_lfo.updateTargetSampleRate(m_targetRate);
        m_ssgHwEnv.updateTargetSampleRate(m_targetRate);
        m_phaseDelta = m_currentFrequency / m_targetRate;
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    m_quantizeSteps = getTargetBitDepth(params.ssg.quality.bit);

    m_pitchResetOnLegato = params.pitchResetOnLegato;
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
    float finalFreq = m_unison.applyDetune(freq);

    // 基本周波数にデチューン成分を加算
    // Save for recalculation
    finalFreq = m_fixMode.noteOn(finalFreq);
    m_currentFrequency = m_detune.noteOn(finalFreq);
    m_phaseDelta = m_currentFrequency / m_targetRate;
    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    if (!isLegato) {
        if (!m_isMonoMode) {
            m_phase = (m_unison.getPhaseOffset() * juce::MathConstants<float>::twoPi);

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= juce::MathConstants<float>::twoPi) {
                m_phase -= juce::MathConstants<float>::twoPi;
            }
            m_lfo.noteOn();
        }

        m_ssgHwEnv.noteOn();
        m_rateAccumulator = 0.0;
        m_lastSample = 0.0f;
    }

    if (!isLegato) {
        m_currentLevel = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.noteOn();
        }

        if (!m_ssgSwEnv11.isBypass()) {
            m_ssgSwEnv11.noteOn();
        }

        if (!m_ssgSwPenv11.isBypass()) {
            m_ssgSwPenv11.noteOn();
        }
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    if (!m_ssgSwPenv11.isBypass() && m_pitchResetOnLegato) {
        m_ssgSwPenv11.noteOn();
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

    if (!m_ssgSwEnv11.isBypass()) {
        m_ssgSwEnv11.noteOff();
    }

    if (!m_ssgSwPenv11.isBypass()) {
        m_ssgSwPenv11.noteOff();
    }
}

bool SsgCore::isPlaying() const { return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying() || m_ssgSwEnv11.isPlaying(); }

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

    m_wtMod.setModWheel(m_modWheel);
}

void SsgCore::setPitchBendRatio(float ratio)
{ m_pitchBendRatio = ratio; }

// --- SsgCore.cpp : getSample() を丸ごと差し替え ---

float SsgCore::getSample()
{
    // すべてのアンプエンベロープがバイパスされているかどうかを判定
    bool isAllAmpBypassed = m_adsr.isBypass() && m_ssgSwEnv.isBypass() && m_ssgSwEnv11.isBypass();

    if (isAllAmpBypassed) {
        // 全てのアンプエンベロープがバイパスの時は、完全な矩形波（Gate）動作
        // どれかが Release 状態（noteOffが呼ばれた直後）なら、即座に音を消す
        if (m_adsr.isRelease() || m_ssgSwEnv.isRelease() || m_ssgSwEnv11.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv.bypassedReleasedProcess();
            m_ssgSwEnv11.bypassedReleasedProcess();
            m_pitchAdsr.bypassedReleasedProcess();
            m_ssgSwPenv11.bypassedReleasedProcess();
            return 0.0f;
        }
    }
    else if (!isPlaying()) {
        // いずれかのアンプエンベロープが有効で、全ての再生が終了（音が減衰しきった）時
        // ピッチエンベロープも強制終了させる（次のノートオンでピッチが変になるのを防ぐ）
        m_pitchAdsr.bypassedReleasedProcess();
        m_ssgSwPenv11.bypassedReleasedProcess();
        return 0.0f;
    }

    float finalEnv = 1.0f;

    if (!isAllAmpBypassed) {
        // 1. ADSR処理
        if (!m_adsr.isBypass()) {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel;
        }
        else {
            if (m_adsr.isRelease()) {
                m_adsr.bypassedReleasedProcess();
            }
            else {
                m_currentLevel = m_adsr.process(m_currentLevel);
                finalEnv *= m_currentLevel;
            }
        }

        // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
        if (!m_ssgSwEnv.isBypass()) {
            finalEnv *= m_ssgSwEnv.process();
        }
        else {
            if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
        }

        // 3. SSG Sw Env 11 処理
        if (!m_ssgSwEnv11.isBypass()) {
            finalEnv *= m_ssgSwEnv11.process();
        }
        else {
            if (m_ssgSwEnv11.isRelease()) m_ssgSwEnv11.bypassedReleasedProcess();
        }
    }

    // --- Sample Rate Emulation ---
    double stepSize = m_targetRate / m_sampleRate;
    m_rateAccumulator += stepSize;

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);
    newPhaseDelta = m_ssgSwPenv11.process(newPhaseDelta);

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
        // MODULATION は搬送波の周波数比として掛ける
        float freqMult = m_pitchBendRatio * opzx7PitchMod * mwPitchMod * m_wtMod.process(newPhaseDelta);

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
        float hwEnvGain = m_ssgHwEnv.process();

        // ==========================================
        // 2. Waveform Generation
        // ==========================================
        float toneSample = 0.0f;
        float fcFluc = 0.0f;

        if (m_waveform == 0) // Pulse
        {
            if (m_dutyFc) {
                float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                // 極端なデューティ比による波形消失を防ぐ最低保証
                float minDuty = phaseInc;
                if (currentDuty < minDuty) currentDuty = minDuty;
                if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

                toneSample = (m_phase < currentDuty) ? 1.0f : -1.0f;
                fcFluc = (m_phase < currentDuty) ? -m_dutyFcFluc * (m_phase / currentDuty) : m_dutyFcFluc * ((m_phase - currentDuty) / (1.0f - currentDuty));
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
        // ピッチエンベロープや PM で 1 サンプルの進みが 1.0 を超えても
        // 破綻しないよう while で回す
        while (m_phase >= 1.0f) m_phase -= 1.0f;
        while (m_phase < 0.0f) m_phase += 1.0f;

        // ==========================================
        // 3. Noise Generator
        // ==========================================
        m_noiseGen.generate();

        // ==========================================
        // 4. Mixing
        // ==========================================
        float toneGain = 1.0f - m_mix;
        float noiseGain = m_mix;
        float rawMixed = (toneSample * m_tone * toneGain) + m_noiseGen.generateSample(noiseGain);

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
            // 0〜1 に詰め替えてから丸めると 0.0 が中央ステップから外れて
            // 直流オフセットになるため、バイポーラのまま量子化する
            finalOut = quantizeSample(rawMixed, m_quantizeSteps);
        }
        else {
            finalOut = rawMixed;
        }

        m_lastSample = finalOut + fcFluc;
    }

    // 線形補間を適用して波形を滑らかに出力する
    // m_rateAccumulator は直近に生成したサンプルからの進み具合を
    // ソースサンプル単位 (0.0〜1.0) で保持しているので、そのまま補間係数になる。
    // prev→last を補間する形なので、出力はソース 1 サンプル分だけ遅れる。
    float fraction = (float)m_rateAccumulator;

    float interpolatedSample = m_prevSample + (m_lastSample - m_prevSample) * fraction;

    return interpolatedSample * finalEnv * m_baseLevel * m_level * 4.0f;
}

void SsgCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

    m_unison.applyPan(basePanL, basePanR);
    sample *= m_unison.getGainComp();

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
