#include "Opzx3Operator.h"
#include "../core/PrValues.h"

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

    // Detune
    // Range: -3 to +3 (Derived from params.detune 0-7)
    // 0-3: Positive (0, +1, +2, +3)
    // 4-7: Negative (0, -1, -2, -3)
    int dtReg = m_params.detune & 7;
    float dtSign = 0.0f;

    // 実機(YM2151/2608)の挙動を模倣するため、定数加算ではなく周波数比例させます。
    // これにより「キーによって周波数値が変わる（高音ほど変化Hzが大きい）」挙動になります。
    // 値は実機の数値を参考に調整した近似値です。
    // 0: 0
    // 1: +/- 0.1% (approx)
    // 2: +/- 0.25%
    // 3: +/- 0.45%
    switch (dtReg)
    {
    case 0: // 0
        // dtSign = 0.0f
        break;
    case 1: // -3
        dtSign = -0.0045f;
        break;
    case 2: // -2
        dtSign = -0.0025f;
        break;
    case 3: // -1
        dtSign = -0.001f;
        break;
    case 4: // 0
        // dtSign = 0.0f
        break;
    case 5: // 1
        dtSign = 0.001f;
        break;
    case 6: // 2
        dtSign = 0.0025f;
        break;
    case 7: // 3
        dtSign = 0.0045f;
    }

    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * dtSign;

    // Multi & Detune
    float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;

    // DT2 (OPM Coarse Detune)
    // YM2151: 0=0, 1=+approx 1.414, 2=+approx 1.58, 3=+approx 1.73
    // 0: x1.0
    // 1: x1.41 (600 cent up)
    // 2: x1.58 (780 cent up)
    // 3: x1.78 (950 cent up)
    float dt2Scale = 1.0f;
    switch (m_params.detune2 & 3) {
    case 0: dt2Scale = 1.0f; break;
    case 1: dt2Scale = 1.414f; break;
    case 2: dt2Scale = 1.581f; break;
    case 3: dt2Scale = 1.781f; break;
    }

    // Final Frequency = (Base + DT1) * MUL * DT2
    float finalFreq = detunedBaseFreq * mul * dt2Scale;

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

    float kslAttenuation = 1.0f;
    if (m_params.keyScaleLevel > 0)
    {
        float octaveDiff = (float)(noteNumber - 48) / 12.0f;
        if (octaveDiff < 0) octaveDiff = 0;
        float dbPerOct = 0.0f;
        switch (m_params.keyScaleLevel) {
        case 1: dbPerOct = 1.5f; break;
        case 2: dbPerOct = 3.0f; break;
        case 3: dbPerOct = 6.0f; break;
        }
        float totalDb = dbPerOct * octaveDiff;
        kslAttenuation = std::pow(10.0f, -totalDb / 20.0f);
    }
    m_targetLevel = velocity * tlGain * kslAttenuation;
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
        envVal *= getSsgEnvelopeLevel(m_ssgPhase);
        m_ssgPhase += (double)m_ssgEgFreq / m_sampleRate;
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
    if (m_params.vibEnable) {
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
    auto isOddQuad = [](float phase)
        {
            return phase < 0.25f || (phase >= 0.5f && phase < 0.75f);
        };

    auto doubleSine = [](float p)
        {
            return std::sin(p * 2.0f);
        };

    auto halfLevelSign = [](float sine)
        {
            if (sine > 0.5f) return 0.5f;

            if (sine < -0.5f) return -0.5f;

            return sine;
        };

    auto triangle = [](float phase)
        {
            float value = phase * 4.0f;

            if (phase < 0.25f) // - 1.0f
            {
                return value;
            }

            if (phase < 0.75f) // - 3.0f
            {
                return 2.0f - value;
            }

            return value - 4.0f; // - 4.0f
        };

    auto dsTriangle = [](float phase)
        {
            float position = phase >= 0.5f ? phase - 0.5f : phase;
            float value = position * 8.0f;

            if (position < 0.125f) // - 1.0f
            {
                return value;
            }

            if (position < 0.375f) // - 3.0f
            {
                return 2.0f - value;
            }

            return value - 4.0f; // - 4.0f
        };

    auto diagram = [](float phase)
        {
            float value = phase * 2.0f;

            if (phase < 0.5f)
            {
                return value;
            }

            return value - 2.0f;
        };

    auto dsDiagram = [](float phase)
        {
            float position = phase >= 0.5f ? phase - 0.5f : phase;
            float value = position * 4.0f;

            if (position < 0.25f)
            {
                return value;
            }

            return value - 2.0f;
        };

    auto absHalfSawUp = [](float phase)
        {
            float value = phase * 2.0f;

            if (phase < 0.5f)
            {
                return value;
            }

            return value - 1.0f;
        };

    auto dsAbsHalfSawUp = [](float phase)
        {
            float position = phase >= 0.5f ? phase - 0.5f : phase;
            float value = phase * 4.0f;

            if (phase < 0.25f)
            {
                return value;
            }

            return value - 1.0f;
        };

    auto reverseSign = [](float phase, float nP)
        {
            // 90度(1/2π)進める
            float s2 = std::sin(phase + 0.5f * juce::MathConstants<float>::pi);

            if (nP < 0.25f) return 1.0f - s2;

            if (nP < 0.5f) return 1.0f + s2;

            if (nP < 0.75f) return -1.0f - s2;

            return s2 - 1.0f;
        };

    auto doubleReverseSine = [](float phase, float nP)
        {
            // 45度(1/4π)進める
            float s2 = std::sin((phase + 0.75 * juce::MathConstants<float>::pi) * 2.0f);

            if (nP < 0.125f)
                return 1.0f + s2;

            if (nP < 0.25f)
                return 1.0f - s2;

            if (nP < 0.375f)
                return s2 - 1.0f;

            return -1.0f - s2;
        };

    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    // 以降の計算はすべて、生ラジアンの phase ではなく、normPhase を使う
    switch (wave) {
        // 0-7: OPZ Compatible Set
    case 0:  return s; // Sine
    case 1:  return (normPhase < 0.5f ? s : 0.0f); // Half Sine
    case 2:  return std::abs(s); // Abs Sine
    case 3:  return (isOddQuad(normPhase) ? std::abs(s) : 0.0f); // Quadra Abs Half Sine
    case 4:  return (normPhase < 0.5f ? doubleSine(p) : 0.0f); // Alt Sine
    case 5:  return (normPhase < 0.5f ? std::abs(doubleSine(p)) : 0.0f); // Alt Abs Sine
    case 6:  return (normPhase < 0.5f ? 1.0f : -1.0f); // Square
    case 7: // Log Saw
    {
        float saw = 1.0f - normPhase * 2.0f;
        return saw * saw * saw;
    }
    case 8: // Pudding Sine
    {
        return halfLevelSign(s);
    }
    case 9: // Half Pudding Sine
    {
        return normPhase < 0.5f ? halfLevelSign(s) : 0.0f;
    }
    case 10: // Abs Pudding Sine
    {
        return std::abs(halfLevelSign(s));
    }
    case 11: // Quad Abs Pudding Sine
    {
        return isOddQuad(normPhase) ? std::abs(halfLevelSign(s)) : 0.0f;
    }
    case 12: return (normPhase < 0.5f ? std::sin(p * 2.0f) * 0.5f : 0.0f); // Mini Alt Sine
    case 13: return (normPhase < 0.5f ? std::abs(std::sin(p * 2.0f)) * 0.5f : 0.0f); // Mini Alt Abs Sine
    case 14: return (normPhase < 0.5f ? 1.0f : 0.0f); // Half Square
    case 15: return 0.0f; // None(Wavetable)
    case 16: return triangle(normPhase); // Triangle
    case 17: return (normPhase < 0.5f ? triangle(normPhase) : 0.0f); // Half Triangle
    case 18: return std::abs(triangle(normPhase)); // Abs Triangle
    case 19: return isOddQuad(normPhase) ? std::abs(triangle(normPhase)) : 0.0f; // Quad Abs Triangle
    case 20: return (normPhase < 0.5f ? dsTriangle(normPhase) : 0.0f); // Alt Triangle
    case 21: return (normPhase < 0.5f ? std::abs(dsTriangle(normPhase)) : 0.0f); // Alt Abs Triangle
    case 22: return isOddQuad(normPhase) ? 1.0 : 0.0; // Quad Half Square
    case 23: return 0.0f; // None(Wavetable)
    case 24: return diagram(normPhase); // Diagram
    case 25: return (normPhase < 0.5f ? diagram(normPhase) : 0.0f); // Half Diagram
    case 26: return absHalfSawUp(normPhase); // Abs Half Saw Up
    case 27: return isOddQuad(normPhase) ? absHalfSawUp(normPhase) : 0.0f; // Quad Abs Half Saw Up
    case 28: return (normPhase < 0.5f ? dsDiagram(normPhase) : 0.0f); // Alt Diagram
    case 29: return (normPhase < 0.5f ? dsAbsHalfSawUp(normPhase) : 0.0f); // Alt Quad Abs Half Saw Up
    case 30: return (normPhase < 0.25f ? 1.0f : 0.0f); // Quad Square
    case 31: // 外部オーディオファイル (PCM)
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
        return s;
    case 32:
        return (normPhase < 0.5f ? std::abs(std::sin(p * 2.0f)) : 0.0f);
    case 33:
    {
        float sign = (normPhase < 0.5f) ? 1.0f : -1.0f;
        return sign * (1.0f - std::pow(1.0f - std::abs(s), 4.0f));
    }
    case 34:
        return (1.0f - normPhase * 2.0f);
    case 35:
        return (normPhase * 2.0f - 1.0f);
    case 36:
        return ((1.0f - normPhase * 2.0f) * 0.5f + s * 0.5f);
    case 37:
        return (normPhase < 0.25f ? 1.0f : -1.0f);
    case 38:
        return (normPhase < 0.125f ? 1.0f : -1.0f);
    case 39:
        return (normPhase < 0.0625f ? 1.0f : -1.0f);
    case 40:
        return std::tanh(s * 5.0f);
    case 41:
        return std::exp(-100.0f * std::pow(normPhase - 0.5f, 2.0f)) * 2.0f - 1.0f;
    case 42:
        return std::sin(p) + std::sin(p * 3.0f) * 0.5f + std::sin(p * 5.0f) * 0.25f;
    case 43:
        return (1.0f - normPhase * 2.0f) * std::sin(p * 4.0f);
    case 44:
        return (1.0f - normPhase * 2.0f) * std::sin(p * 8.0f);
    case 45:
    {
        float tri = (normPhase < 0.5f ? (4.0f * normPhase - 1.0f) : (3.0f - 4.0f * normPhase));
        return tri * std::sin(p * 3.0f);
    }
    case 46:
        return s * s * s;
    case 47:
        return std::sin(p) * std::sin(p * 2.0f);
    case 48:
        return s + 0.5f * std::sin(p * 2.0f) + 0.25f * std::sin(p * 4.0f);
    case 49:
        return s * std::cos(p * 2.5f);
    case 50:
        return std::sin(p) * std::sin(p * 1.414f);
    case 51:
        return std::sin(p) * std::cos(p * 0.5f);
    case 52:
        return std::sin(p * 13.0f) * std::cos(p * 7.0f) * std::sin(p * 2.0f);
    case 53: // PD Resonance (Casio CZ Style)
        return (1.0f - std::cos(p)) * std::sin(p * 5.0f) * 0.5f;
    case 54: // PD Resonance High
        return (1.0f - std::cos(p)) * std::sin(p * 9.0f) * 0.5f;
    case 55: // 4-Step Sine (Super Lo-Fi)
        return std::round(s * 2.0f) / 2.0f;
    case 56: // 8-Step Sine (Slightly Lo-Fi)
        return std::round(s * 4.0f) / 4.0f;
    case 57: // Wavefolded Sine (Soft)
    {
        float fs = s * 1.5f; // 1.5倍に増幅
        // 1.0を超えたら折り返す、-1.0を下回っても折り返す
        if (fs > 1.0f) return 2.0f - fs;
        if (fs < -1.0f) return -2.0f - fs;
        return fs;
    }
    case 58: // Wavefolded Sine (Hard / Double fold)
    {
        float fs = s * 2.5f; // さらに増幅して2回折り返す
        return std::sin(fs * juce::MathConstants<float>::halfPi); // サイン関数を使うと滑らかに何度も折り返せます
    }
    case 59: // Bitwise XOR Fractal
    {
        uint8_t phaseInt = (uint8_t)(normPhase * 255.0f);
        // 位相と「位相を1ビットずらした値」をXOR合成
        uint8_t xorVal = phaseInt ^ (phaseInt >> 1);
        // 0.0 ~ 1.0 に戻して -1.0 ~ 1.0 にスケール
        return ((float)xorVal / 255.0f) * 2.0f - 1.0f;
    }
    case 60: // Bitwise AND Texture
    {
        uint8_t phaseInt = (uint8_t)(normPhase * 255.0f);
        uint8_t andVal = phaseInt & (phaseInt << 1);
        return ((float)andVal / 255.0f) * 2.0f - 1.0f;
    }
    case 61: // Self-Modulated Sine (Feedback = 1)
        return std::sin(p + 1.0f * std::sin(p));
    case 62: // Self-Modulated Sine (Feedback = 2, slightly harsh)
        return std::sin(p + 2.0f * std::sin(p));
    case 63: // OPZ(TX81Z) W2
        return reverseSign(p, normPhase); // Reverse Sine
    case 64: // OPZ(TX81Z) W3
        return normPhase < 0.5f ? s : 0.0f; // Half Sine;
    case 65: // OPZ(TX81Z) W4
        return normPhase < 0.5f ? reverseSign(p, normPhase) : 0.0f; // Half Reverse Sine
    case 66: // OPZ(TX81Z) W5
        return normPhase < 0.5f ? doubleSine(p) : 0.0f; // Alt Sine
    case 67: // OPZ(TX81Z) W6
        return normPhase < 0.5f ? doubleReverseSine(p, normPhase) : 0.0f; // Alt Reverse Sine
    case 68: // OPZ(TX81Z) W7
        return normPhase < 0.5f ? std::abs(doubleSine(p)) : 0.0f; // Abs Alt Sine
    case 69: // OPZ(TX81Z) W8
        return normPhase < 0.5f ? std::abs(doubleReverseSine(p, normPhase)) : 0.0f; // Abs Alt Reverse Sine
    case 70: // Cubic Triangle
    {
        float tri = triangle(normPhase);
        return tri * tri * tri; // 3乗することで凹型の鋭い針になる
    }
    case 71: // Inverse Circle
    {
        float tri = triangle(normPhase);
        float absTri = std::abs(tri);
        float sign = (tri >= 0.0f) ? 1.0f : -1.0f;
        // 1.0 - sqrt(1.0 - x^2) で、頂点が限界まで尖ったカーブを作る
        return sign * (1.0f - std::sqrt(std::max(0.0f, 1.0f - absTri * absTri)));
    }
    case 72: // Exponential Spike
    {
        // 前半(0.0~0.5)と後半(0.5~1.0)でローカルな位相(0.0~1.0)を作る
        float localPhase = std::fmod(normPhase * 2.0f, 1.0f);
        float sign = (normPhase < 0.5f) ? 1.0f : -1.0f;

        // exp(-x) で一気に減衰させる。8.0fは尖り具合（大きいほど細い針になる）
        return sign * std::exp(-localPhase * 8.0f);
    }
    default: return s;
    }
}
