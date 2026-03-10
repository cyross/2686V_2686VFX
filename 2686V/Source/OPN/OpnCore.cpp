#include "OpnCore.h"
#include "../synth/SynthHelpers.h"

void OpnCore::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;

    updateNoiseDelta(target);

    m_amSmooth = 0.0f;
}

void OpnCore::setParameters(const SynthParams& params)
{
    m_algorithm = params.algorithm;
    m_lfoWave = params.lfoWave;
    m_amSmoothRate = params.lfoAmSmRt;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);

        updateNoiseDelta(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.feedback;
        }
        else if (i == 2) // OP2
        {
            fb = params.feedback2;
        }

        // OPN: SSG-EG=False, WaveSelect=False
        m_operators[i].setParameters(params.fmOp[i], fb, false, false);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void OpnCore::noteOn(float freq, float velocity)
{
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);
    m_rateAccumulator = 1.0;
}

void OpnCore::noteOff()
{
    for (auto& op : m_operators) op.noteOff();
}

bool OpnCore::isPlaying() const
{
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void OpnCore::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    // 全オペレーターに適用
    for (auto& op : m_operators) op.setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void OpnCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float OpnCore::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        // ノイズの更新処理
        if (m_lfoPhase >= 1.0) {
            m_lfoPhase -= 1.0;
            unsigned int bit0 = m_lfsr & 1;
            unsigned int bit3 = (m_lfsr >> 3) & 1;
            unsigned int nextBit = bit0 ^ bit3;
            m_lfsr >>= 1;
            if (nextBit) m_lfsr |= (1 << 16);
            m_currentNoiseSample = ((m_lfsr % 1000) / 500.0f) - 1.0f;
        }

        float amLfoVal = 0.0f;
        float pmLfoVal = 0.0f;

        switch (m_lfoWave) {
        case 0: // Sine
            pmLfoVal = (float)std::sin(m_lfoPhase * 2.0 * juce::MathConstants<double>::pi);
            amLfoVal = (pmLfoVal + 1.0f) * 0.5f;
            break;
        case 1: // Saw Down
            pmLfoVal = (float)(1.0 - m_lfoPhase * 2.0);
            amLfoVal = (float)(1.0 - m_lfoPhase);
            break;
        case 2: // Square
            pmLfoVal = (m_lfoPhase < 0.5) ? 1.0f : -1.0f;
            amLfoVal = (m_lfoPhase < 0.5) ? 1.0f : 0.0f;
            break;
        case 3: // Triangle
            if (m_lfoPhase < 0.25)       pmLfoVal = (float)(m_lfoPhase * 4.0);
            else if (m_lfoPhase < 0.75)  pmLfoVal = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
            else                         pmLfoVal = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

            if (m_lfoPhase < 0.5)        amLfoVal = (float)(m_lfoPhase * 2.0);
            else                         amLfoVal = (float)(1.0 - (m_lfoPhase - 0.5) * 2.0);
            break;
        case 4: // Noise
            pmLfoVal = m_currentNoiseSample;
            amLfoVal = (m_currentNoiseSample + 1.0f) * 0.5f;
            break;
        }

        m_amSmooth += (amLfoVal - m_amSmooth) * m_amSmoothRate;

        float out1, out2, out3, out4;
        float finalOut = 0.0f;

        // 各getSample呼び出しに lfoPitchMod を渡すように修正
        m_operators[0].getSample(out1, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

        if (m_opMask[0]) out1 = 0.0f; // Mask

        switch (m_algorithm) {
        case 0:
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 1:
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out1 + out2, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 2:
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out3 + out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out4;

            break;
        case 3: // OP1->OP2->OP4, OP3->OP4 (出力は4のみ)
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out2 + out3, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4; // 出力は4のみ

            break;
        case 4: // OP1->OP2, OP3->OP4 (出力は2と4)
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out4; // 出力は2と4のみ

            break;
        case 5:
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out2 + out3 + out4;

            break;
        case 6:
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out2 + out3 + out4;

            break;
        default:
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f; // Mask

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f; // Mask

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, false, false, 0, 0, -1.0f, -1.0f, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f; // Mask

            finalOut = out1 + out2 + out3 + out4;

            break;
        }

        // =======================================================
        // 複数のキャリアが加算されても1.0を超えないように音量を調整
        // =======================================================
        finalOut *= 0.25f;

        // =======================================================
        // 無音(0.0)が完全に0.0になるBipolar(双極性)量子化
        // =======================================================
        if (m_quantizeSteps > 0.0f) {
            // 単純に掛け算して丸め、割り算で戻すだけでゼロクロスが保証されます
            finalOut = std::round(finalOut * m_quantizeSteps) / m_quantizeSteps;

            // 安全のためのクリップ
            finalOut = std::clamp(finalOut, -1.0f, 1.0f);
        }

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    return m_prevSample + (m_lastSample - m_prevSample) * fraction;
}
