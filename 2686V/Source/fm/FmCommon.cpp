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

    // Fixed Mode
    float baseFreq = frequency;
    if (m_params.fixedMode) baseFreq = m_params.fixedFreq;

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
    juce::Logger::getCurrentLogger()->writeToLog("DT:" + juce::String(dtReg));
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

// --- Processing ---
// output: 結果書き込み先
// modulator: FM変調入力
// lfoAmp: AM変調 (1.0 = 変調なし)
// lfoPitch: PM変調 (1.0 = 変調なし)
void FmOperator::getSample(float& output, float modulator, float lfoAmp, float lfoPitch)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    // 1. ADSR Update
    updateEnvelopeState();

    // 2. Calculate Envelope Value
    // ここではエンベロープの形状(0.0-1.0)のみを取得し、
    // 最終的な音量は出力段で掛けます。
    float envVal = m_currentLevel;

    // SSG-EG (OPNA Only)
    if (m_useSsgEg && m_params.ssgEg > 0) {
        envVal *= getSsgEnvelopeLevel(m_ssgPhase);

        // 音程(m_phaseDelta)ではなく、設定した周波数(m_ssgEgFreq)で進める
        // これにより、音程に関わらず一定の速度でシュワシュワします
        double phaseInc = (double)m_ssgEgFreq / m_sampleRate;
        m_ssgPhase += phaseInc;
    }

    // LFO AM (OPNA Only)
    if (m_params.amEnable) {
        envVal *= lfoAmp;
    }

    // 3. Phase & Waveform
    // Feedback
    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        // 実機のFB(0〜7)の掛かり具合に近似させる
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    // LFO PM (OPNA Only)
    float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio;

    currentPhaseDelta *= lfoPitch;

    // YMチップ実機と同等のFM変調の深さ（モジュレーションインデックス）を設定
    // Modulator に対して、FM変調の最大インデックスを実機のスケール(約4π)に設定
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    // 変調波(modulator)にインデックスを掛けてから足す
    float modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackMod;

    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    // Wave Select (OPL Only / Standard Sine)
    // ここで m_targetLevel (Velocity/TL) を適用します！
    // これにより、Attackが1.0に張り付くスパイクノイズが解消されます。
    output = rawWave * envVal * m_targetLevel;

    // 外部フィードバックモードでない場合のみ、自身の出力を格納する
    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = output;
    }

    // Phase Advance
    m_phase += currentPhaseDelta;
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}

float FmOperator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    if (!m_useWaveSelect) return s; // Default Sine (OPN/OPNA/OPM)

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    // 以降の計算はすべて、生ラジアンの phase ではなく、normPhase を使う
    switch (wave) {
        // 0-7: OPZ / OPL3 Compatible Set
    case 0:  return s; // Sine
    case 1:  return (normPhase < 0.5f ? s : 0.0f); // Half Sine
    case 2:  return std::abs(s); // Abs Sine
    case 3:
        if (m_useSsgEg) return (p < 0.5f * juce::MathConstants<float>::pi) ? s : 0.0f; // Pulse
        else return (normPhase < 0.25f ? s : 0.0f); // Quarter Sine
    case 4:  return (normPhase < 0.5f ? std::sin(p * 2.0f) : 0.0f);
    case 5:  return (normPhase < 0.5f ? std::abs(std::sin(p * 2.0f)) : 0.0f);
    case 6:  return (normPhase < 0.5f ? 1.0f : -1.0f); // Square
    case 7:
    {
        float sign = (normPhase < 0.5f) ? 1.0f : -1.0f;
        return sign * (1.0f - std::pow(1.0f - std::abs(s), 4.0f));
    }
    // 8-12: Saw / Tri Variations
    case 8:  return (1.0f - normPhase * 2.0f); // Saw Down
    case 9:  return (normPhase * 2.0f - 1.0f); // Saw Up
    case 10: // Triangle
        return (normPhase < 0.5f ? (4.0f * normPhase - 1.0f) : (3.0f - 4.0f * normPhase));
    case 11: // Saw + Sine combined
        return ((1.0f - normPhase * 2.0f) * 0.5f + s * 0.5f);
    case 12: // Log Saw
    {
        float saw = 1.0f - normPhase * 2.0f;
        return saw * saw * saw;
    }
    // 13-18: Pulse / Square Variations
    case 13: return (normPhase < 0.25f ? 1.0f : -1.0f); // Pulse 25%
    case 14: return (normPhase < 0.125f ? 1.0f : -1.0f); // Pulse 12.5%
    case 15: return (normPhase < 0.0625f ? 1.0f : -1.0f); // Pulse 6.25%
    case 16: // Rounded Square
        return std::tanh(s * 5.0f);
    case 17: // Impulse train
        return std::exp(-100.0f * std::pow(normPhase - 0.5f, 2.0f)) * 2.0f - 1.0f;
    case 18: // Comb
        return std::sin(p) + std::sin(p * 3.0f) * 0.5f + std::sin(p * 5.0f) * 0.25f;
        // 19-28: Resonant / Complex Waves
    case 19: return (1.0f - normPhase * 2.0f) * std::sin(p * 4.0f);
    case 20: return (1.0f - normPhase * 2.0f) * std::sin(p * 8.0f);
    case 21:
    {
        float tri = (normPhase < 0.5f ? (4.0f * normPhase - 1.0f) : (3.0f - 4.0f * normPhase));
        return tri * std::sin(p * 3.0f);
    }
    case 22: return s * s * s; // Bulb Sine
    case 23: return std::sin(p) * std::sin(p * 2.0f); // Double Hump
    case 24: return s + 0.5f * std::sin(p * 2.0f) + 0.25f * std::sin(p * 4.0f);
    case 25: return s * std::cos(p * 2.5f);
    case 26: return std::sin(p) * std::sin(p * 1.414f);
    case 27: return std::sin(p) * std::cos(p * 0.5f);
    case 28: return std::sin(p * 13.0f) * std::cos(p * 7.0f) * std::sin(p * 2.0f);
    case 29: // 外部オーディオファイル (PCM)
        if (m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
        {
            // ここも normPhase を使って計算
            float floatIndex = normPhase * m_pcmBuffer->size();
            int index1 = (int)floatIndex;
            int index2 = (index1 + 1) % m_pcmBuffer->size();
            float frac = floatIndex - (float)index1;
            return (*m_pcmBuffer)[index1] * (1.0f - frac) + (*m_pcmBuffer)[index2] * frac;
        }
        return s;
    default: return s;
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
