#pragma once
#include "FmCommon.h"

// ==========================================================
// OPL (YM3526/3812) Core
// Features: 2 Operators, 2 Algorithms (FM/AM), Wave Select
// ==========================================================
class OplCore
{
public:
    OplCore() {}

    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);
        m_rateAccumulator = 1.0;
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm; // 0:Serial(FM), 1:Parallel(AM)

        if (m_rateIndex != params.fmRateIndex) {
            m_rateIndex = params.fmRateIndex;
            double target = getTargetRate();
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

        // OP1: Feedbackあり, OP2: なし
        // OPL: SSG-EG=False, WaveSelect=True
        m_operators[0].setParameters(params.fmOp[0], params.feedback, false, true);
        m_operators[1].setParameters(params.fmOp[1], 0.0f,            false, true);
    }

    void noteOn(float freq, float velocity) {
        float gain = std::max(0.01f, velocity);
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        m_operators[0].noteOn(freq, gain, noteNum);
        m_operators[1].noteOn(freq, gain, noteNum);
        m_rateAccumulator = 1.0;
    }

    void noteOff() {
        m_operators[0].noteOff();
        m_operators[1].noteOff();
    }

    bool isPlaying() const { return m_operators[0].isPlaying() || m_operators[1].isPlaying(); }

    float getSample() {
        double targetRate = getTargetRate();
        m_rateAccumulator += targetRate / m_hostSampleRate;

        if (m_rateAccumulator >= 1.0) {
            m_rateAccumulator -= 1.0;

            float out1, out2;
            m_operators[0].getSample(out1, 0.0f);

            float finalOut = 0.0f;
            if (m_algorithm == 0) {
                m_operators[1].getSample(out2, out1);
                finalOut = out2;
            }
            else {
                m_operators[1].getSample(out2, 0.0f);
                finalOut = (out1 + out2) * 0.5f;
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
private:
    double getTargetRate() const {
        switch (m_rateIndex) {
        case 0: return 96000.0;
        case 1: return 55500.0;
        case 2: return 48000.0;
        case 3: return 44100.0;
        case 4: return 22050.0;
        case 5: return 16000.0;
        case 6: return 8000.0;
        default: return 55500.0;
        }
    }

    std::array<FmOperator, 2> m_operators;
    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;
};
