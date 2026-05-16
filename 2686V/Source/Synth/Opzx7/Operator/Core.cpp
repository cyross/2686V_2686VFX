#include "./Core.h"
#include "../../../Processor/Opzx7/Values.h"

namespace {
    // =================================================================
    // 波形生成用のユーティリティ関数群
    // =================================================================
    inline bool isOddQuad(float phase) {
        return phase < 0.25f || (phase >= 0.5f && phase < 0.75f);
    }

    inline float doubleSine(float p) {
        return std::sin(p * 2.0f);
    }

    inline float halfLevelSign(float sine) {
        if (sine > 0.5f) return 0.5f;
        if (sine < -0.5f) return -0.5f;
        return sine;
    }

    inline float triangle(float phase) {
        float value = phase * 4.0f;
        if (phase < 0.25f) return value;
        if (phase < 0.75f) return 2.0f - value;
        return value - 4.0f;
    }

    inline float dsTriangle(float phase) {
        float position = phase >= 0.5f ? phase - 0.5f : phase;
        float value = position * 8.0f;
        if (position < 0.125f) return value;
        if (position < 0.375f) return 2.0f - value;
        return value - 4.0f;
    }

    inline float diagram(float phase) {
        float value = phase * 2.0f;
        if (phase < 0.5f) return value;
        return value - 2.0f;
    }

    inline float dsDiagram(float phase) {
        float position = phase >= 0.5f ? phase - 0.5f : phase;
        float value = position * 4.0f;
        if (position < 0.25f) return value;
        return value - 2.0f;
    }

    inline float absHalfSawUp(float phase) {
        float value = phase * 2.0f;
        if (phase < 0.5f) return value;
        return value - 1.0f;
    }

    inline float dsAbsHalfSawUp(float phase) {
        float position = phase >= 0.5f ? phase - 0.5f : phase;
        float value = position * 4.0f;
        if (position < 0.25f) return value;
        return value - 1.0f;
    }

    inline float reverseSign(float phase, float nP) {
        float s2 = std::sin(phase + 0.5f * juce::MathConstants<float>::pi);
        if (nP < 0.25f) return 1.0f - s2;
        if (nP < 0.5f) return 1.0f + s2;
        if (nP < 0.75f) return -1.0f - s2;
        return s2 - 1.0f;
    }

    inline float doubleReverseSine(float phase, float nP) {
        float s2 = std::sin((phase + 0.75f * juce::MathConstants<float>::pi) * 2.0f);
        if (nP < 0.125f) return 1.0f + s2;
        if (nP < 0.25f) return 1.0f - s2;
        if (nP < 0.375f) return s2 - 1.0f;
        return -1.0f - s2;
    }

    // =================================================================
    // 波形ストラテジー配列の定義
    // (引数: ラジアン位相 p, 正規化位相 n, サイン波 s)
    // =================================================================
    using WaveCalculator = float(*)(float p, float n, float s);

    const std::array<WaveCalculator, 73> waveStrategies = { {
        // 0: Sine
        [](float p, float n, float s) { return s; },
        // 1: Half Sine
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 2: Abs Sine
        [](float p, float n, float s) { return std::abs(s); },
        // 3: Quadra Abs Half Sine
        [](float p, float n, float s) { return isOddQuad(n) ? std::abs(s) : 0.0f; },
        // 4: Alt Sine
        [](float p, float n, float s) { return n < 0.5f ? doubleSine(p) : 0.0f; },
        // 5: Alt Abs Sine
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleSine(p)) : 0.0f; },
        // 6: Square
        [](float p, float n, float s) { return n < 0.5f ? 1.0f : -1.0f; },
        // 7: Log Saw
        [](float p, float n, float s) { float saw = 1.0f - n * 2.0f; return saw * saw * saw; },
        // 8: Pudding Sine
        [](float p, float n, float s) { return halfLevelSign(s); },
        // 9: Half Pudding Sine
        [](float p, float n, float s) { return n < 0.5f ? halfLevelSign(s) : 0.0f; },
        // 10: Abs Pudding Sine
        [](float p, float n, float s) { return std::abs(halfLevelSign(s)); },
        // 11: Quad Abs Pudding Sine
        [](float p, float n, float s) { return isOddQuad(n) ? std::abs(halfLevelSign(s)) : 0.0f; },
        // 12: Mini Alt Sine
        [](float p, float n, float s) { return n < 0.5f ? std::sin(p * 2.0f) * 0.5f : 0.0f; },
        // 13: Mini Alt Abs Sine
        [](float p, float n, float s) { return n < 0.5f ? std::abs(std::sin(p * 2.0f)) * 0.5f : 0.0f; },
        // 14: Half Square
        [](float p, float n, float s) { return n < 0.5f ? 1.0f : 0.0f; },
        // 15: Wavetable (Dummy: 実際の処理はメソッド内で分岐)
        [](float p, float n, float s) { return 0.0f; },
        // 16: Triangle
        [](float p, float n, float s) { return triangle(n); },
        // 17: Half Triangle
        [](float p, float n, float s) { return n < 0.5f ? triangle(n) : 0.0f; },
        // 18: Abs Triangle
        [](float p, float n, float s) { return std::abs(triangle(n)); },
        // 19: Quad Abs Triangle
        [](float p, float n, float s) { return isOddQuad(n) ? std::abs(triangle(n)) : 0.0f; },
        // 20: Alt Triangle
        [](float p, float n, float s) { return n < 0.5f ? dsTriangle(n) : 0.0f; },
        // 21: Alt Abs Triangle
        [](float p, float n, float s) { return n < 0.5f ? std::abs(dsTriangle(n)) : 0.0f; },
        // 22: Quad Half Square
        [](float p, float n, float s) { return isOddQuad(n) ? 1.0f : 0.0f; },
        // 23: None
        [](float p, float n, float s) { return 0.0f; },
        // 24: Diagram
        [](float p, float n, float s) { return diagram(n); },
        // 25: Half Diagram
        [](float p, float n, float s) { return n < 0.5f ? diagram(n) : 0.0f; },
        // 26: Abs Half Saw Up
        [](float p, float n, float s) { return absHalfSawUp(n); },
        // 27: Quad Abs Half Saw Up
        [](float p, float n, float s) { return isOddQuad(n) ? absHalfSawUp(n) : 0.0f; },
        // 28: Alt Diagram
        [](float p, float n, float s) { return n < 0.5f ? dsDiagram(n) : 0.0f; },
        // 29: Alt Quad Abs Half Saw Up
        [](float p, float n, float s) { return n < 0.5f ? dsAbsHalfSawUp(n) : 0.0f; },
        // 30: Quad Square
        [](float p, float n, float s) { return n < 0.25f ? 1.0f : 0.0f; },
        // 31: PCM (Dummy: 実際の処理はメソッド内で分岐)
        [](float p, float n, float s) { return 0.0f; },
        // 32: 
        [](float p, float n, float s) { return n < 0.5f ? std::abs(std::sin(p * 2.0f)) : 0.0f; },
        // 33:
        [](float p, float n, float s) { float sign = (n < 0.5f) ? 1.0f : -1.0f; return sign * (1.0f - std::pow(1.0f - std::abs(s), 4.0f)); },
        // 34:
        [](float p, float n, float s) { return 1.0f - n * 2.0f; },
        // 35:
        [](float p, float n, float s) { return n * 2.0f - 1.0f; },
        // 36:
        [](float p, float n, float s) { return (1.0f - n * 2.0f) * 0.5f + s * 0.5f; },
        // 37:
        [](float p, float n, float s) { return n < 0.25f ? 1.0f : -1.0f; },
        // 38:
        [](float p, float n, float s) { return n < 0.125f ? 1.0f : -1.0f; },
        // 39:
        [](float p, float n, float s) { return n < 0.0625f ? 1.0f : -1.0f; },
        // 40:
        [](float p, float n, float s) { return std::tanh(s * 5.0f); },
        // 41:
        [](float p, float n, float s) { return std::exp(-100.0f * std::pow(n - 0.5f, 2.0f)) * 2.0f - 1.0f; },
        // 42:
        [](float p, float n, float s) { return std::sin(p) + std::sin(p * 3.0f) * 0.5f + std::sin(p * 5.0f) * 0.25f; },
        // 43:
        [](float p, float n, float s) { return (1.0f - n * 2.0f) * std::sin(p * 4.0f); },
        // 44:
        [](float p, float n, float s) { return (1.0f - n * 2.0f) * std::sin(p * 8.0f); },
        // 45:
        [](float p, float n, float s) { float tri = (n < 0.5f ? (4.0f * n - 1.0f) : (3.0f - 4.0f * n)); return tri * std::sin(p * 3.0f); },
        // 46:
        [](float p, float n, float s) { return s * s * s; },
        // 47:
        [](float p, float n, float s) { return std::sin(p) * std::sin(p * 2.0f); },
        // 48:
        [](float p, float n, float s) { return s + 0.5f * std::sin(p * 2.0f) + 0.25f * std::sin(p * 4.0f); },
        // 49:
        [](float p, float n, float s) { return s * std::cos(p * 2.5f); },
        // 50:
        [](float p, float n, float s) { return std::sin(p) * std::sin(p * 1.414f); },
        // 51:
        [](float p, float n, float s) { return std::sin(p) * std::cos(p * 0.5f); },
        // 52:
        [](float p, float n, float s) { return std::sin(p * 13.0f) * std::cos(p * 7.0f) * std::sin(p * 2.0f); },
        // 53: PD Resonance (Casio CZ Style)
        [](float p, float n, float s) { return (1.0f - std::cos(p)) * std::sin(p * 5.0f) * 0.5f; },
        // 54: PD Resonance High
        [](float p, float n, float s) { return (1.0f - std::cos(p)) * std::sin(p * 9.0f) * 0.5f; },
        // 55: 4-Step Sine
        [](float p, float n, float s) { return std::round(s * 2.0f) / 2.0f; },
        // 56: 8-Step Sine
        [](float p, float n, float s) { return std::round(s * 4.0f) / 4.0f; },
        // 57: Wavefolded Sine (Soft)
        [](float p, float n, float s) { float fs = s * 1.5f; if (fs > 1.0f) return 2.0f - fs; if (fs < -1.0f) return -2.0f - fs; return fs; },
        // 58: Wavefolded Sine (Hard)
        [](float p, float n, float s) { float fs = s * 2.5f; return std::sin(fs * juce::MathConstants<float>::halfPi); },
        // 59: Bitwise XOR Fractal
        [](float p, float n, float s) { uint8_t pi = (uint8_t)(n * 255.0f); uint8_t xv = pi ^ (pi >> 1); return ((float)xv / 255.0f) * 2.0f - 1.0f; },
        // 60: Bitwise AND Texture
        [](float p, float n, float s) { uint8_t pi = (uint8_t)(n * 255.0f); uint8_t av = pi & (pi << 1); return ((float)av / 255.0f) * 2.0f - 1.0f; },
        // 61: Self-Modulated Sine (FB=1)
        [](float p, float n, float s) { return std::sin(p + 1.0f * std::sin(p)); },
        // 62: Self-Modulated Sine (FB=2)
        [](float p, float n, float s) { return std::sin(p + 2.0f * std::sin(p)); },
        // 63: OPZ(TX81Z) W2
        [](float p, float n, float s) { return reverseSign(p, n); },
        // 64: OPZ(TX81Z) W3
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 65: OPZ(TX81Z) W4
        [](float p, float n, float s) { return n < 0.5f ? reverseSign(p, n) : 0.0f; },
        // 66: OPZ(TX81Z) W5
        [](float p, float n, float s) { return n < 0.5f ? doubleSine(p) : 0.0f; },
        // 67: OPZ(TX81Z) W6
        [](float p, float n, float s) { return n < 0.5f ? doubleReverseSine(p, n) : 0.0f; },
        // 68: OPZ(TX81Z) W7
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleSine(p)) : 0.0f; },
        // 69: OPZ(TX81Z) W8
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleReverseSine(p, n)) : 0.0f; },
        // 70: Cubic Triangle
        [](float p, float n, float s) { float tri = triangle(n); return tri * tri * tri; },
        // 71: Inverse Circle
        [](float p, float n, float s) { float tri = triangle(n); float absTri = std::abs(tri); float sign = (tri >= 0.0f) ? 1.0f : -1.0f; return sign * (1.0f - std::sqrt(std::max(0.0f, 1.0f - absTri * absTri))); },
        // 72: Exponential Spike
        [](float p, float n, float s) { float lp = std::fmod(n * 2.0f, 1.0f); float sign = (n < 0.5f) ? 1.0f : -1.0f; return sign * std::exp(-lp * 8.0f); }
    } };
}

void Opzx7Operator::prepare(double sampleRate) {
    m_pitchAdsr.prepare(sampleRate);
    m_lfo.prepare(sampleRate);
}

void Opzx7Operator::updateSampleRate(double newSampleRate) {
    m_pitchAdsr.updateSampleRate(newSampleRate);
    m_lfo.updateSampleRate(newSampleRate);
}

void Opzx7Operator::setParameters(const FmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = params.fmSsgEgFreq;
    m_params.waveSelect = params.waveSelect;
    m_detune.setParameters(params.detune, params.detune2, params.multiple, params.mutipleRatio);
    m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_lfo.setParameters(params.lfoSyncDelay, params.vibEnable, params.amEnable, params.lfoFreq, params.lfoFreq, params.pgLfoWave, params.egLfoWave, params.pms, params.pmd, params.ams, params.amd, 0.01f);
}

void Opzx7Operator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = m_params.phaseOffset;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

    m_lfo.noteOn();

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    if (m_params.waveSelect == Opzx7PrValue::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        // オフセットとレシオを考慮した再生区間の計算
        size_t totalSize = m_pcmBuffer->size();
        size_t offsetSamples = (size_t)((m_params.pcmOffset / 1000.0f) * m_hostSampleRate);
        if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

        size_t remainingSize = totalSize - offsetSamples;
        size_t playSize = (size_t)(remainingSize * m_params.pcmRatio);
        if (playSize < 1) playSize = 1;

        float originalHz = (float)m_hostSampleRate / (float)playSize;

        if (m_params.fixedMode) {
            baseFreq = originalHz * m_params.fixedFreq;
        }
        else {
            float rootFreq = 261.625565f;
            float pitchRatio = frequency / rootFreq;
            baseFreq = originalHz * pitchRatio;
        }
    }
    else
    {
        baseFreq = m_fixMode.noteOn(frequency);
    }

    // 基本周波数にデチューン成分を加算
    float finalFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    // TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = m_params.rtl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    m_targetLevel = velocity * tlGain;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;

    m_pitchAdsr.noteOn();
}

void Opzx7Operator::noteOff()
{
    // XOFが有効なときはノートオフ処理を無効化
    if (m_params.xofEnable)
    {
        return;
    }

    m_state = State::Release;

    // キーを離した瞬間、SUSがONならSRを維持する
    if (m_params.susEnable) {
        m_currentReleaseDec = 0.0f;
    }
    else {
        m_currentReleaseDec = m_releaseDec;
    }

    m_pitchAdsr.noteOff();
}

//void Opzx7Operator::getSample(float& output, float modulator,
//    float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd, float globalAmd, float modWheel)
void Opzx7Operator::getSample(float& output, float modulator, Opzx7LfoCore& glLfo, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    if (m_params.ssgEg > 0) {
        int safeWave = std::clamp(m_params.ssgEg, 0, 15);

        envVal *= ssgWaveStrategies[safeWave](m_ssgPhase);
        m_ssgPhase += (double)m_ssgEgFreq / m_sampleRate;
    }

    // Sync Delay 更新
    if (m_lfoDelayCounter > 0.0f) {
        m_lfoDelayCounter -= 1.0f / (float)m_sampleRate;
        if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;
    }

    // ========================================================
    // 0. ローカル LFO の計算 (波形独立)
    // ========================================================
    m_lfo.getSample();

    // ========================================================
    // 1. Amplitude Modulation (Tremolo) の連続計算
    // ========================================================
    float totalAmpMod = 1.0f;

    // ① グローバルAM (最大 96dB の減衰)
    if (glLfo.amEnable) {
        totalAmpMod *= std::pow(10.0f, -(glLfo.value.am * glLfo.depthDb) / 20.0f);
    }

    if (m_lfo.amEnable) {
        totalAmpMod *= std::pow(10.0f, -(m_lfo.value.am * m_lfo.depthDb) / 20.0f);
    }

    envVal *= totalAmpMod;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の連続計算
    // ========================================================
    float currentPitchCent = 1.0f;

    // ① グローバルPM (最大 ±1200 Cent の揺れ幅 = ±1オクターブ)
    if (glLfo.pmEnable) {
        currentPitchCent += glLfo.value.pm * glLfo.depthCent;
    }

    if (m_lfo.pmEnable) {
        currentPitchCent += m_lfo.value.pm * m_lfo.depthCent;
    }

    // ③ モジュレーションホイール (Global LFO を使用、最大200セント)
    currentPitchCent += glLfo.value.pm * (modWheel * 200.0f);

    // 蓄積した Cent を周波数倍率に変換
    float lfoPitchMod = std::pow(2.0f, currentPitchCent / 1200.0f);

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

	float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitchMod;
    float currentPhaseDelta = m_params.pitchEnvEnable ? m_pitchAdsr.process(basePhaseDelta) : basePhaseDelta;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    if (m_params.waveSelect == Opzx7PrValue::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        size_t totalSize = m_pcmBuffer->size();
        size_t offsetSamples = (size_t)((m_params.pcmOffset / 1000.0f) * m_hostSampleRate);
        if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;
        size_t remainingSize = totalSize - offsetSamples;
        size_t playSize = (size_t)(remainingSize * m_params.pcmRatio);
        if (playSize < 1) playSize = 1;

        float originalHz = (float)m_hostSampleRate / (float)playSize;

        float scale = originalHz / 261.625565f;
        scale = std::max(scale, 0.005f);
        fmModIndex *= scale;
    }

    float modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackMod;

    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    output = rawWave * envVal * m_targetLevel;

    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = output;
    }

    m_phase += currentPhaseDelta;
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}

float Opzx7Operator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);

    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波と正規化位相を計算
    float s = std::sin(p);
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    // =================================================================
    // PCM波形の特別処理 (メンバ変数へのアクセスが必要なため分離)
    // =================================================================
    if (wave == 31)
    {
        if (m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
        {
            // OffsetとRatioを適用して切り出す
            size_t totalSize = m_pcmBuffer->size();
            size_t offsetSamples = (size_t)((m_params.pcmOffset / 1000.0f) * m_hostSampleRate);

            if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

            size_t remainingSize = totalSize - offsetSamples;
            size_t playSize = (size_t)(remainingSize * m_params.pcmRatio);

            if (playSize < 1) playSize = 1;

            // normPhase (0.0 ~ 1.0) を playSize にマッピングし、offsetを足す
            float floatIndex = (float)offsetSamples + normPhase * (float)playSize;
            int index1 = (int)floatIndex;

            if (index1 >= totalSize) index1 = totalSize - 1;

            int index2 = index1 + 1;

            // ループの終端に達したら、offset位置（先頭）に戻す
            if (index2 >= offsetSamples + playSize || index2 >= totalSize) {
                index2 = offsetSamples;
            }

            float frac = floatIndex - (float)index1;

            return (*m_pcmBuffer)[index1] * (1.0f - frac) + (*m_pcmBuffer)[index2] * frac;
        }

        return s; // PCMバッファが無い場合はサイン波を返す
    }

    // =================================================================
    // 波形メモリの特別処理 (メンバ変数へのアクセスが必要なため分離)
    // =================================================================
    if (m_params.waveSelect == Opzx7PrValue::wtIndex)
    {
        if (m_wtBuffer != nullptr && !m_wtBuffer->empty())
        {
            size_t totalSize = m_wtBuffer->size();

            // normPhase (0.0 ~ 1.0) を totalSize にマッピング
            float floatIndex = normPhase * (float)totalSize;
            int index1 = (int)floatIndex;

            if (index1 >= totalSize) index1 = totalSize - 1;

            int index2 = index1 + 1;
            if (index2 >= totalSize) index2 = 0; // ループ

            float frac = floatIndex - (float)index1;

            // 線形補間で滑らかに読み取る
            return (*m_wtBuffer)[index1] * (1.0f - frac) + (*m_wtBuffer)[index2] * frac;
        }

        return s; // データが無い場合はサイン波を返す
    }

    // =================================================================
    // ストラテジー配列による高速な波形計算
    // =================================================================
    int safeWave = std::clamp(wave, 0, 72);

    return waveStrategies[safeWave](p, normPhase, s);
}

void Opzx7Operator::updateIncrementsWithKeyScale()
{
    if (m_sampleRate <= 0.0) return;

    // ====================================================================
    // レジスタモード (RG-EN = ON) : 実機のアルゴリズムで増減量を計算
    // ====================================================================
    if (m_params.regEnable)
    {
        // 1. キースケールレート (KSR) の算出
        int ksrValue = 0;

        int octave = (m_noteNumber / 12) - 1;
        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        if (m_params.isOplMode)
        {
            int noteOffset = m_noteNumber % 12;
            int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
            ksrValue = m_params.keyScale > 0 ? keyRate : (keyRate >> 2);
        }
        else
        {
            int noteOffset = m_noteNumber % 12;
            int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
            ksrValue = keyRate >> (3 - std::clamp(m_params.keyScale, 0, 3));
        }

        // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
        // isRRフラグを追加し、RRの時だけスケールを調整する
        auto calcRegRate = [&](int regVal, bool isRR) -> float {
            // RR以外のRate0は停止（サステイン維持など）。
            if (regVal == 0 && !isRR) return 0.0f;

            int baseRate = regVal;

            // OPL系（全て4bit）、およびOPN系のRR（4bit）は、5bit(0-31)スケールに補正する
            if (m_params.isOplMode || isRR) {
                // 15の時に31になるように (val * 2 + 1)
                baseRate = (regVal * 2) + 1;
            }

            // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
            // 非常にゆっくり（約20秒）減衰して消えるようにする。
            if (baseRate <= 1 && isRR) {
                return 1.0f / (20.0f * (float)m_sampleRate);
            }

            // 実効レート = 基本レート(0-31) * 2 + KSR (0-3)
            int effectiveRate = (baseRate * 2) + ksrValue;
            if (effectiveRate > 63) effectiveRate = 63;

            float timeInSeconds = 0.0f;
            if (effectiveRate >= 60) {
                // Rate 60以上はほぼ瞬時（1ミリ秒）
                timeInSeconds = 0.001f;
            }
            else {
                // レートが4下がるごとに時間が約2倍になる実機カーブの近似
                float powFactor = (60.0f - (float)effectiveRate) / 4.0f;
                timeInSeconds = 0.0015f * std::pow(2.0f, powFactor);
            }

            return 1.0f / (timeInSeconds * (float)m_sampleRate);
            };

        // 各レートの計算（第2引数に、それがRRかどうかのフラグを渡す）
        m_attackInc = calcRegRate(m_params.rar, false);
        m_decayDec = calcRegRate(m_params.rdr, false);
        m_sustainRateDec = (m_params.rsr == 0) ? 0.0f : calcRegRate(m_params.rsr, false);

        // Release Rate の計算時のみ isRR を true にする
        m_releaseDec = calcRegRate(m_params.rrr, true);

        m_susReleaseDec = calcRegRate(5, true);

        // 3. サステインレベル (SL) の計算
        if (m_params.rsl == 15) {
            m_params.sustain = 0.0f; // SL=15 は一気に0まで落ちる
        }
        else {
            // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
            float slDb = m_params.rsl * 3.0f;
            m_params.sustain = std::pow(10.0f, -slDb / 20.0f);
        }
    }
    // ====================================================================
    // 従来モード (RG-EN = OFF) : 既存の秒数ベースの計算
    // ====================================================================
    else
    {
        // KeyScaleによるスケーリング計算のバグを修正
        float rateScale = 1.0f;
        if (m_params.keyScale > 0) {
            // m_noteNumber(通常0〜127) を使ってスケールを計算するが、
            // 係数を小さくして急激な倍率変化を防ぐ
            float noteFactor = (float)(m_noteNumber) / 127.0f;
            rateScale = 1.0f + ((float)m_params.keyScale * noteFactor * 0.5f);
        }

        // param(秒数) に対してスケーリングを行う。
        // param が 0 の時（0.001fの時）に正しく 1ms になるように計算式を修正。
        auto calcInc = [&](float paramInSeconds) -> float {
            // スケールを適用した実際の秒数（短くなる）
            float scaledSeconds = paramInSeconds / rateScale;
            // 最低でも 1ms (0.001秒) は保証する
            float finalSeconds = std::max(0.001f, scaledSeconds);
            // サンプルレートから「1サンプルあたりに進む量」を返す
            return 1.0f / (finalSeconds * (float)m_sampleRate);
            };

        m_attackInc = calcInc(m_params.attack);
        m_decayDec = calcInc(m_params.decay);
        m_releaseDec = calcInc(m_params.release);

        m_susReleaseDec = calcInc(1.5f);

        if (m_params.sustainRate <= 0.001f) {
            m_sustainRateDec = 0.0f;
        }
        else {
            // Sustain Rate は値(0.0~1.0)が小さいほど遅い（長い）という特殊な仕様
            float srTime = 5.0f * (1.0f - m_params.sustainRate);
            m_sustainRateDec = calcInc(srTime);
        }

        // 従来モードのサステインレベルはそのまま適用する
        m_params.sustain = m_params.sustain;
    }
}
