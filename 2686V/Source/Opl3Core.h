#pragma once
#include "FmCommon.h"

// ==========================================================
// OPL3 (YMF262) Core
// Features: 4 Operators, Wave Select (8 types), 4-Op algorithms
// ==========================================================
class Opl3Core
{
public:
    Opl3Core() {}
    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);
        m_rateAccumulator = 1.0;
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;

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

        for(int i=0; i<4; ++i) {
            float fb = (i == 0) ? params.feedback : 0.0f;
            m_operators[i].setParameters(params.fmOp[i], fb, false, true);
        }
    }

    void noteOn(float freq, float velocity) {
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        for(auto& op : m_operators) op.noteOn(freq, velocity, noteNum);

        m_rateAccumulator = 1.0;
    }
    void noteOff() { for(auto& op : m_operators) op.noteOff(); }
    bool isPlaying() const { return m_operators[0].isPlaying(); }

    float getSample() {
        double targetRate = getTargetRate();
        m_rateAccumulator += targetRate / m_hostSampleRate;

        if (m_rateAccumulator >= 1.0) {
            m_rateAccumulator -= 1.0;

            float out1, out2, out3, out4;
            m_operators[0].getSample(out1, 0.0f);

            float finalOut = 0.0f;
            switch (m_algorithm) {
            case 0: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, out2); m_operators[3].getSample(out4, out3); finalOut = out4; break;
            case 1: m_operators[1].getSample(out2, 0);    m_operators[2].getSample(out3, out1 + out2); m_operators[3].getSample(out4, out3); finalOut = out4; break;
            case 2: m_operators[1].getSample(out2, 0);    m_operators[2].getSample(out3, out2); m_operators[3].getSample(out4, out3 + out1); finalOut = out4; break;
            case 3: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, out3); finalOut = out2 + out4; break;
            case 4: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, 0); finalOut = out2 + out3 + out4; break;
            case 5: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, out1); m_operators[3].getSample(out4, out1); finalOut = out2 + out3 + out4; break;
            case 6: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, 0); finalOut = out2 + out3 + out4; break;
            default: m_operators[1].getSample(out2, 0); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, 0); finalOut = out1 + out2 + out3 + out4; break;
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

    std::array<FmOperator, 4> m_operators;
    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;
};