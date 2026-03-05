#include "FmCommon.h"

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

    if (m_useWaveSelect && m_params.waveSelect == 29 && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        // 1回ループするのに必要な「本来の周波数（等速のHz）」を計算
        float originalHz = (float)m_sampleRate / (float)m_pcmBuffer->size();

        if (m_params.fixedMode) {
            // FIXモード時: fixedFreq(1.0等) を「再生速度の倍率」として扱う
            baseFreq = originalHz * m_params.fixedFreq;
        }
        else {
            // キーボードモード時: C4 (Note 60, 約261.626Hz) を Root Key(等速) としてピッチを変化
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
    if (m_params.totalLevel < 0.99f) {
        // totalLevel(0.0〜1.0) を 0dB 〜 -96dB に変換
        float attenuationDb = m_params.totalLevel * 96.0f;
        tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
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

    if (m_useWaveSelect && m_params.waveSelect == 29 && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        // 波形の長さから「本来の周波数（等速のHz）」を計算
        float originalHz = (float)m_sampleRate / (float)m_pcmBuffer->size();

        // 基準となる中音域(C4 = 約261.6Hz)に対して、波形がどれくらい長いかでスケールダウンする
        float scale = originalHz / 261.625565f;

        // 極端に小さくなりすぎないよう、最低限の変調は残す
        scale = std::max(scale, 0.005f);

        // FM変調インデックスを波形の長さに合わせて縮小
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
void FmOperator::getSample(float& output, float modulator, float lfoVal,
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
        float lfoAmpMod = 1.0f - (std::abs(lfoVal) * totalAmDepth);
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

    // ③ モジュレーションホイール (MIDI演奏のため常に足し込む)
    float wheelDepth = modWheel * 0.03f;
    totalPmDepth += wheelDepth;

    float lfoPitchMod = 1.0f + (lfoVal * totalPmDepth);

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

    if (m_useWaveSelect && m_params.waveSelect == 29 && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        // 波形の長さから「本来の周波数（等速のHz）」を計算
        float originalHz = (float)m_sampleRate / (float)m_pcmBuffer->size();

        // 基準となる中音域(C4 = 約261.6Hz)に対して、波形がどれくらい長いかでスケールダウンする
        float scale = originalHz / 261.625565f;

        // 極端に小さくなりすぎないよう、最低限の変調は残す
        scale = std::max(scale, 0.005f);

        // FM変調インデックスを波形の長さに合わせて縮小
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
        case 20: return (normPhase < 0.5f ? dsTriangle(normPhase) : 0.0f); // Double Speed Triangle
        case 21: return (normPhase < 0.5f ? std::abs(dsTriangle(normPhase)) : 0.0f); // Double Speed Triangle
        case 22: return isOddQuad(normPhase) ? 1.0 : 0.0; // Quad Half Square
        case 23: return 0.0f; // None(Wavetable)
        case 24: return diagram(normPhase); // Diagram
        case 25: return (normPhase < 0.5f ? diagram(normPhase) : 0.0f); // Half Diagram
        case 26: return absHalfSawUp(normPhase); // Abs Half Saw Up
        case 27: return isOddQuad(normPhase) ? absHalfSawUp(normPhase) : 0.0f; // Quad Abs Half Saw Up
        case 28: return (normPhase < 0.5f ? dsDiagram(normPhase) : 0.0f); // Double Speed Diagram
        case 29: return (normPhase < 0.5f ? dsAbsHalfSawUp(normPhase) : 0.0f); // Double Speed Diagram
        case 30: return (normPhase < 0.25f ? 1.0f : 0.0f); // Quad Square
        case 31: // 外部オーディオファイル (PCM)
            if (m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
            {
                // ここも normPhase を使って計算
                float floatIndex = normPhase * m_pcmBuffer->size();
                int index1 = (int)floatIndex;

                // ==========================================================
                // 浮動小数点の誤差や変調による範囲外アクセスを防ぐ絶対安全ガード
                // ==========================================================
                if (index1 >= m_pcmBuffer->size()) index1 = (int)m_pcmBuffer->size() - 1;
                if (index1 < 0) index1 = 0;

                int index2 = (index1 + 1) % m_pcmBuffer->size();
                float frac = floatIndex - (float)index1;
                return (*m_pcmBuffer)[index1] * (1.0f - frac) + (*m_pcmBuffer)[index2] * frac;
            }
            return s;
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
        // Decayのターゲット判定ロジックを変更
        // 以前: float limitLevel = m_targetLevel * m_params.sustain;
        // 修正: エンベロープは0-1で正規化されたので、sustain値(0-1)と比較するだけでOK
        float limitLevel = m_params.sustain;
        if (m_currentLevel > limitLevel) {
            m_currentLevel -= m_decayDec;
            if (m_currentLevel <= limitLevel) {
                m_currentLevel = limitLevel;
                m_state = State::Sustain;
            }
        }
        else {
            m_currentLevel = limitLevel;
            m_state = State::Sustain;
        }
    }
    else if (m_state == State::Sustain) {
        // サステインフェーズのロジック

        // 1. OPL/OPL3のパーカッシブモード (EG TYPE = OFF)
        // egType == false の場合、RR (Release Rate) を使って即座に減衰する
        if (!m_params.egType) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
        // 2. OPN/OPM/OPZX3の持続・減衰モード、または OPL/OPLLの持続モード (EG TYPE = ON)
        else {
            // SR (Sustain Rate) が設定されていれば、それに従って減衰する
            // (OPL/OPLL等で SR=0 に固定されている場合は減衰せず維持される)
            if (m_sustainRateDec > 0.0f) {
                m_currentLevel -= m_sustainRateDec;
                if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
            }
        }
    }
    else if (m_state == State::Release) {
        m_currentLevel -= m_releaseDec;
        if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
    }
}

void FmOperator::updateIncrementsWithKeyScale()
{
    if (m_sampleRate <= 0.0) return;
    float rateScale = 1.0f;
    if (m_params.keyScale > 0) {
        float noteFactor = (float)(m_noteNumber) / 128.0f;
        rateScale = 1.0f + ((float)m_params.keyScale * noteFactor * 2.0f);
    }
    auto calcInc = [&](float param) { return 1.0f / (float)(std::max(0.001f, param / rateScale) * m_sampleRate); };
    m_attackInc = calcInc(m_params.attack);
    m_decayDec = calcInc(m_params.decay);
    m_releaseDec = calcInc(m_params.release);

    if (m_params.sustainRate <= 0.001f)
    {
        m_sustainRateDec = 0.0f;
    }
    else
    {
        // 値が大きいほど速く減衰する
        // 簡易計算: 0.001(param) -> 5.0s, 1.0(param) -> 0.005s
        float srTime = std::max(0.001f, 5.0f * (1.0f - m_params.sustainRate));
        m_sustainRateDec = 1.0f / (float)((srTime / rateScale) * m_sampleRate);
    }
}
