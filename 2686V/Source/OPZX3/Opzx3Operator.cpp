#include "Opzx3Operator.h"
#include "../core/PrValues.h"

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
        // 15: None
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

void Opzx3Operator::setParameters(const FmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = params.fmSsgEgFreq;
    m_params.waveSelect = params.waveSelect;
}

void Opzx3Operator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = m_params.phaseOffset;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

    m_lfoDelayCounter = m_params.lfoSyncDelay / 1000.0f;

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    if (m_params.waveSelect == PrValue::Opzx3::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
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
    else if(m_params.fixedMode)
    {
        baseFreq = m_params.fixedFreq;
    }

    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * dtScales[m_params.detune & 7];

    // Multi & Detune
    float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;

    // Final Frequency = (Base + DT1) * MUL * DT2
    float finalFreq = detunedBaseFreq * mul * dt2Scales[m_params.detune2 & 3];

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    // TL Calculation
    float tlGain = 0.0f;
    if (m_params.regEnable)
    {
        // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
        // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
        float attenuationDb = m_params.rtl * 0.75f;
        tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
    }
    else
    {
        // 従来モード: 0.0〜1.0 を 0dB〜-96dB にマッピング
        if (m_params.totalLevel < 0.99f) {
            float attenuationDb = m_params.totalLevel * 96.0f;
            tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
        }
        else {
            tlGain = 0.0f;
        }
    }

    m_targetLevel = velocity * tlGain;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;
}

void Opzx3Operator::getSample(float& output, float modulator, float amLfoVal, float pmLfoVal,
    bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd, float globalAmd, float modWheel)
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
        m_lfoDelayCounter -= 1.0f / (float)m_hostSampleRate;
        if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;
    }

    // ========================================================
    // 1. Amplitude Modulation (Tremolo / Wah) の計算
    // ========================================================
    float amsDepths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
    float totalAmDepth = 0.0f;

    // ① グローバルAM (G-AMスイッチがONの時のみ受け取る)
    if (globalAm) {
        float globalDepthScale = (globalAmd >= 0.0f) ? (globalAmd / 127.0f) : 1.0f;
        totalAmDepth += amsDepths[std::clamp(globalAms, 0, 3)] * globalDepthScale;
    }

    // ② ローカルAM (このOP独自の揺れ深さ)
    if (m_params.amEnable) {
        totalAmDepth += amsDepths[std::clamp(m_params.ams, 0, 3)];
    }

    // 上限を1.0(100%)でクリップ
    totalAmDepth = std::min(totalAmDepth, 1.0f);

    if (totalAmDepth > 0.0f) {
        float lfoAmpMod = 1.0f - (amLfoVal * totalAmDepth);
        envVal *= lfoAmpMod; // 音量に直接適用
    }

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    float pmsDepths[] = { 0.0f, 0.003f, 0.006f, 0.012f, 0.03f, 0.06f, 0.26f, 0.5f };
    float totalPmDepth = 0.0f;

    // ① グローバルPM (G-PMスイッチがONの時のみ受け取る)
    if (globalPm) {
        float globalPmdScale = (globalPmd >= 0.0f) ? (globalPmd / 127.0f) : 1.0f;
        totalPmDepth += pmsDepths[std::clamp(globalPms, 0, 7)] * globalPmdScale;
    }

    // ② ローカルPM (このOP独自の揺れ深さ)
    if (m_params.vibEnable && m_lfoDelayCounter <= 0.0f) {
        totalPmDepth += pmsDepths[std::clamp(m_params.pms, 0, 7)];
    }

    // PMがONの時だけ、その深さをLFO波形に掛ける
    float currentPitchMod = pmLfoVal * totalPmDepth;

    // ③ モジュレーションホイール (MIDI演奏のため常に足し込む)
    float wheelDepth = modWheel * 0.03f;
    currentPitchMod += (pmLfoVal * wheelDepth);

    float lfoPitchMod = 1.0f + currentPitchMod;

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitchMod;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    if (m_params.waveSelect == PrValue::Opzx3::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
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

float Opzx3Operator::calcWaveform(double phase, int wave)
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
    // ストラテジー配列による高速な波形計算
    // =================================================================
    int safeWave = std::clamp(wave, 0, 72);

    return waveStrategies[safeWave](p, normPhase, s);
}
