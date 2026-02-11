#pragma once
#include "FmCommon.h"

// ==========================================================
// OPNA (YM2608) Core
// Features: 4 Operators, 8 Algorithms, SSG-EG, Hardware LFO
// ==========================================================
class OpnaCore
{
public:
    OpnaCore() {}

    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;

        // Update operators with current target rate
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);

        m_lfoPhase = 0.0;
        m_rateAccumulator = 1.0; // Ready to render
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;
        m_lfoFreq = params.lfoFreq;
        m_pms = params.pms;
        m_ams = params.ams;

        if (m_rateIndex != params.fmRateIndex) {
            m_rateIndex = params.fmRateIndex;
            double target = getTargetRate();
            for (auto& op : m_operators) op.setSampleRate(target);
        }

        switch (params.fmBitDepth) {
        case 0: m_quantizeSteps = 15.0f; break; // 4bit
        case 1: m_quantizeSteps = 31.0f; break; // 5bit
        case 2: m_quantizeSteps = 63.0f; break; // 6bit
        case 3: m_quantizeSteps = 255.0f; break; // 8bit
        case 4: m_quantizeSteps = 0.0f; break;   // Raw
        default: m_quantizeSteps = 0.0f; break;
        }

        for (int i = 0; i < 4; ++i) {
            float fb = (i == 0) ? params.feedback : 0.0f;
            // OPNA: SSG-EG=True, WaveSelect=False
            m_operators[i].setParameters(params.fmOp[i], fb, true, false);
        }
    }

    void noteOn(float freq, float velocity) {
        float gain = std::max(0.01f, velocity);
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        for (auto& op : m_operators) op.noteOn(freq, gain, noteNum);

        m_rateAccumulator = 1.0; // Reset timing
    }

    void noteOff() { for (auto& op : m_operators) op.noteOff(); }
    bool isPlaying() const { return m_operators[0].isPlaying(); }

    float getSample() {
        // --- Rate Emulation ---
        double targetRate = getTargetRate();
        m_rateAccumulator += targetRate / m_hostSampleRate;

        if (m_rateAccumulator >= 1.0) {
            m_rateAccumulator -= 1.0;

            // --- Actual Synthesis (Run at Target Rate) ---

            // LFO Update (Using Target Rate)
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;
            if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

            float lfoValue = 0.0f;
            if (m_lfoPhase < 0.25)      lfoValue = (float)(m_lfoPhase * 4.0);
            else if (m_lfoPhase < 0.75) lfoValue = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
            else                        lfoValue = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

            // AMS
            float lfoAmpMod = 1.0f;
            if (m_ams > 0) {
                float depths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
                lfoAmpMod = 1.0f - (std::abs(lfoValue) * depths[m_ams & 3]);
            }

            // PMS
            float lfoPitchMod = 1.0f;
            if (m_pms > 0) {
                float depths[] = { 0.0f, 0.001f, 0.002f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f };
                lfoPitchMod = 1.0f + (lfoValue * depths[m_pms & 7]);
            }

            // Routing
            float out1, out2, out3, out4;
            float finalOut = 0.0f;

            // Operators update their state here
            m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

            switch (m_algorithm) {
            case 0: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); finalOut = out4; break;
            case 1: m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);    m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); finalOut = out4; break;
            case 2: m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);    m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3 + out1, lfoAmpMod, lfoPitchMod); finalOut = out4; break;
            case 3: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); finalOut = out2 + out4; break;
            case 4: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); finalOut = out2 + out3 + out4; break;
            case 5: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out1, lfoAmpMod, lfoPitchMod); finalOut = out2 + out3 + out4; break;
            case 6: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); finalOut = out2 + out3 + out4; break;
            default:
                m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); finalOut = out1 + out2 + out3 + out4; break;
            }

            // Quantization
            if (m_quantizeSteps > 0.0f) {
                // Clamp first to avoid blowing up logic
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
    double m_hostSampleRate = 44100.0;
    int m_algorithm = 0;

    // Rate & Quality
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;
    // LFO State
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    int m_pms = 0;
    int m_ams = 0;
};
