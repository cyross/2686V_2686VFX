#include "FmCommon.h"
#include "../core/PrValues.h"

void FmOperator::setParameters(const FmOpParams& params, float feedback, bool useSsgEg, bool useWaveSelect, bool useOpmEg, float ssgEgFreq)
{
    m_params = params;
    m_feedback = feedback;
    m_useSsgEg = useSsgEg;
    m_ssgEgFreq = ssgEgFreq;
    m_useWaveSelect = useWaveSelect;
    m_useOpmEg = useOpmEg;

    // OPNA以外の場合はSSG-EGパラメータを無視
    if (!m_useSsgEg) m_params.ssgEg = 0;

    // OPNA/OPNの場合はWaveSelectを無視(Sine固定)
    if (m_useWaveSelect) {
        m_params.waveSelect = params.waveSelect;
    }
    else
    {
        m_params.waveSelect = 0;
    }
}

void FmOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = 0.0;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    m_currentLevel = 0.0f;

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    if (m_useWaveSelect && m_params.waveSelect == PrValue::Opzx3::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
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
        // --- 通常の波形（Sineなど）の場合 ---
        if (m_params.fixedMode) {
            baseFreq = m_params.fixedFreq;
        }
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

void FmOperator::noteOff()
{
    m_state = State::Release;

    // ★追加: キーを離した瞬間、SUSがONならリリースレートをゆっくり(RR=5)にする
    if (m_params.susEnable) {
        m_currentReleaseDec = m_susReleaseDec;
    }
    else {
        m_currentReleaseDec = m_releaseDec;
    }
}

// =====================================================================
// ① 直接指定版 (OPL / OPL3 等用)
// =====================================================================
void FmOperator::getSample(float& output, float modulator, float lfoAmp, float lfoPitch)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    if (m_useSsgEg && m_params.ssgEg > 0) {
        envVal *= getSsgEnvelopeLevel(m_ssgPhase);
        double phaseInc = (double)m_ssgEgFreq / m_sampleRate;
        m_ssgPhase += phaseInc;
    }

    // AM適用 (無条件。変調がない場合はコア側から 1.0 が渡ってくる)
    envVal *= lfoAmp;

    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    // PM適用 (無条件。変調がない場合は 1.0 が渡ってくる)
    float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitch;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    if (m_useWaveSelect && m_params.waveSelect == PrValue::Opzx3::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
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

    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect, true);
    output = rawWave * envVal * m_targetLevel;

    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = output;
    }

    m_phase += currentPhaseDelta;
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}

// =====================================================================
// ② ハイブリッドLFO版 (OPNA / OPM / OPZX3 / OPN 用)
// =====================================================================
void FmOperator::getSample(float& output, float modulator, float amLfoVal, float pmLfoVal,
    bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd, float globalAmd, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    if (m_useSsgEg && m_params.ssgEg > 0) {
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

    if (m_useWaveSelect && m_params.waveSelect == PrValue::Opzx3::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
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

    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect, false);

    output = rawWave * envVal * m_targetLevel;

    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = output;
    }

    m_phase += currentPhaseDelta;
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}

float FmOperator::calcWaveform(double phase, int wave, bool isOpl)
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

    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    if (!m_useWaveSelect) return s; // Default Sine (OPN/OPNA/OPM)

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    // 以降の計算はすべて、生ラジアンの phase ではなく、normPhase を使う
    if (isOpl)
    {
        // OPL/OPL3
        switch (wave) {
            // 0-7: OPZ / OPL3 Compatible Set
        case 0:  return s; // Sine
        case 1:  return (normPhase < 0.5f ? s : 0.0f); // Half Sine
        case 2:  return std::abs(s); // Abs Sine
        case 3:
            if (m_useSsgEg) return (p < 0.5f * juce::MathConstants<float>::pi) ? s : 0.0f; // Pulse
            else return (normPhase < 0.25f ? s : 0.0f); // Quarter Sine
        case 4:  return (normPhase < 0.5f ? doubleSine(p) : 0.0f);
        case 5:  return (normPhase < 0.5f ? std::abs(doubleSine(p)) : 0.0f);
        case 6:  return (normPhase < 0.5f ? 1.0f : -1.0f); // Square
        case 7:
        {
            float sign = (normPhase < 0.5f) ? 1.0f : -1.0f;
            return sign * (1.0f - std::pow(1.0f - std::abs(s), 4.0f));
        }
        default: return s;
        }
    }
    else
    {
        // OPZX3
        switch (wave) {
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
        default: return s;
        }
    }
}

float FmOperator::getSsgEnvelopeLevel(double p)
{
    int cycle = (int)p;
    double subPos = p - cycle;
    bool isEven = (cycle % 2 == 0);
    switch (m_params.ssgEg) {
    case 2:  return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
    case 4:  return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
    case 6: return isEven ? (1.0f - (float)subPos) : (float)subPos;
    case 8: return (cycle == 0) ? (1.0f - (float)subPos) : 1.0f;
    case 9: return (cycle == 0) ? (float)subPos : 0.0f;
    case 11: return (cycle == 0) ? (float)subPos : 1.0f;
    case 13: return isEven ? (float)subPos : (1.0f - (float)subPos);
    case 15: return (cycle == 0) ? (float)subPos : 0.0f;
    default: return 1.0f;
    }
}

void FmOperator::updateEnvelopeState()
{
    if (m_state == State::Attack) {
        m_currentLevel += m_attackInc;
        if (m_currentLevel >= 1.0f) { m_currentLevel = 1.0f; m_state = State::Decay; }
    }
    else if (m_state == State::Decay) {
        float limitLevel = m_params.sustain;

        if (m_currentLevel > limitLevel) {
            // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
            if (m_decayDec > 0.0f) {
                m_currentLevel -= m_decayDec;
                if (m_currentLevel <= limitLevel) {
                    m_currentLevel = limitLevel;
                    m_state = State::Sustain;
                }
            }
        }
        else {
            m_currentLevel = limitLevel;
            m_state = State::Sustain;
        }
    }
    else if (m_state == State::Sustain) {
        // ====================================================================
        // パーカッシブモード(EG-TYP=OFF)の判定を OPL/OPL3 系統のみに限定する
        // ====================================================================
        if (m_params.isOplMode && !m_params.egType) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
        // OPN/OPM/OPZX3、または OPL系の持続モード(EG-TYP=ON) の場合
        else {
            // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
            if (m_sustainRateDec > 0.0f) {
                m_currentLevel -= m_sustainRateDec;
                if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
            }
        }
    }
    else if (m_state == State::Release) {
        m_currentLevel -= m_currentReleaseDec;
        if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
    }
}
void FmOperator::updateIncrementsWithKeyScale()
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
