#include "Opl3Core.h"
#include "../synth/SynthHelpers.h"

void Opl3Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) op.setSampleRate(target);
    m_rateAccumulator = 1.0;
}

void Opl3Core::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
    }

    switch (params.fmBitDepth) {
    case 0: m_quantizeSteps = 15.0f; break;
    case 1: m_quantizeSteps = 31.0f; break;
    case 2: m_quantizeSteps = 63.0f; break;
    case 3: m_quantizeSteps = 255.0f; break;
    case 4: m_quantizeSteps = 0.0f; break;
    default: m_quantizeSteps = 0.0f; break;
    }

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

        m_operators[i].setParameters(params.fmOp[i], fb, false, true);
        m_opMask[i] = params.fmOp[i].mask;
    }
}

void Opl3Core::noteOn(float freq, float velocity) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);

    m_rateAccumulator = 1.0;
}
void Opl3Core::noteOff() { for (auto& op : m_operators) op.noteOff(); }
bool Opl3Core::isPlaying() const {
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}
// ピッチベンド (0 - 16383, Center=8192)
void Opl3Core::setPitchBend(int pitchWheelValue)
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
void Opl3Core::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float Opl3Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);
    m_rateAccumulator += targetRate / m_hostSampleRate;

    if (m_rateAccumulator >= 1.0) {
        m_rateAccumulator -= 1.0;

        // LFO Logic (Run at Target Rate)
        m_amPhase += (3.7 / targetRate);
        if (m_amPhase >= 1.0) m_amPhase -= 1.0;

        float amVal = 0.0f;

        if (m_amPhase < 0.25) {
            amVal = (float)(m_amPhase * 4.0);
        }
        else if (m_amPhase < 0.75)
        {
            amVal = (float)(1.0 - (m_amPhase - 0.25) * 4.0);
        }
        else
        {
            amVal = (float)(-1.0 + (m_amPhase - 0.75) * 4.0);
        }

        float lfoAmpVal = 1.0f - (0.5f * (amVal + 1.0f));

        m_vibPhase += (6.4 / targetRate);
        if (m_vibPhase >= 1.0) m_vibPhase -= 1.0;

        float vibVal = 0.0f;

        if (m_vibPhase < 0.25) {
            vibVal = (float)(m_vibPhase * 4.0);
        }
        else if (m_vibPhase < 0.75)
        {
            vibVal = (float)(1.0 - (m_vibPhase - 0.25) * 4.0);
        }
        else
        {
            vibVal = (float)(-1.0 + (m_vibPhase - 0.75) * 4.0);
        }

        float pmDepth = 0.03f * m_modWheel; // +/- 3% (approx 50cents)
        float lfoPitchVal = 1.0f + (vibVal * pmDepth);

        // -------------------------------

        float out1, out2, out3, out4;
        float finalOut = 0.0f;

        // getSample に lfoPitchMod を渡す
        m_operators[0].getSample(out1, 0.0f, lfoAmpVal, lfoPitchVal);

        if (m_opMask[0]) out1 = 0.0f; // Mask

        switch (m_algorithm) {
        case 0:
            m_operators[1].getSample(out2, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, out2, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out3, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out4;
            break;
        case 1:
            m_operators[1].getSample(out2, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, out1 + out2, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out3, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out4;
            break;
        case 2:
            m_operators[1].getSample(out2, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, out2, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out3 + out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out4;
            break;
        case 3:
            m_operators[1].getSample(out2, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out3, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out2 + out4;
            break;
        case 4:
            m_operators[1].getSample(out2, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out2 + out3 + out4;
            break;
        case 5:
            m_operators[1].getSample(out2, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out2 + out3 + out4;
            break;
        case 6:
            m_operators[1].getSample(out2, out1, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out2 + out3 + out4;
            break;
        default:
            m_operators[1].getSample(out2, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[1]) out2 = 0.0f; // Mask
            m_operators[2].getSample(out3, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[2]) out3 = 0.0f; // Mask
            m_operators[3].getSample(out4, 0, lfoAmpVal, lfoPitchVal);
            if (m_opMask[3]) out4 = 0.0f; // Mask
            finalOut = out1 + out2 + out3 + out4;
            break;
        }

        if (m_quantizeSteps > 0.0f) {
            if (finalOut > 1.0f) finalOut = 1.0f; else if (finalOut < -1.0f) finalOut = -1.0f;
            float norm = (finalOut + 1.0f) * 0.5f;
            float quantized = std::floor(norm * m_quantizeSteps) / m_quantizeSteps;
            m_lastSample = (quantized * 2.0f) - 1.0f;
        }
        else {
            m_lastSample = finalOut;
        }
    }
    return m_lastSample;
}
