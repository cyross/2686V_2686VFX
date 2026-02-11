#pragma once
#include "FmCommon.h"
#include <random>

class OpmCore
{
public:
    OpmCore() {}
    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
        double target = getTargetRate();
        for (auto& op : m_operators) op.setSampleRate(target);
        m_lfoPhase = 0.0;
        m_rateAccumulator = 1.0;

        updateNoiseDelta(target);
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;
        m_lfoFreq = params.lfoFreq;
        m_pms = params.pms;
        m_ams = params.ams;
        m_lfoWave = params.lfoWave; // OPM LFO Wave

        if (m_rateIndex != params.fmRateIndex) {
            m_rateIndex = params.fmRateIndex;
            double target = getTargetRate();
            for (auto& op : m_operators) op.setSampleRate(target);
            updateNoiseDelta(target);
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
            float fb = (i == 0) ? params.feedback : 0.0f;
            // OPM: SSG-EG=False, WaveSelect=False
            m_operators[i].setParameters(params.fmOp[i], fb, false, false, true);
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

            // --- OPM LFO (at Target Rate) ---
            double lfoInc = m_lfoFreq / targetRate;
            m_lfoPhase += lfoInc;
            if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

            float lfoVal = 0.0f;

            switch (m_lfoWave) {
            case 0: lfoVal = (float)(1.0 - m_lfoPhase * 2.0); break; // Saw Down
            case 1: lfoVal = (m_lfoPhase < 0.5) ? 1.0f : -1.0f; break; // Square
            case 2: // Triangle
                if (m_lfoPhase < 0.25) lfoVal = m_lfoPhase * 4.0f;
                else if (m_lfoPhase < 0.75) lfoVal = 1.0f - (m_lfoPhase - 0.25f) * 4.0f;
                else lfoVal = -1.0f + (m_lfoPhase - 0.75f) * 4.0f;
                break;
            case 3: // Noise LFO
                m_noisePhase += m_noiseDelta;
                if (m_noisePhase >= 1.0f) {
                    m_noisePhase -= 1.0f;
                    unsigned int bit0 = m_lfsr & 1;
                    unsigned int bit3 = (m_lfsr >> 3) & 1;
                    unsigned int nextBit = bit0 ^ bit3;
                    m_lfsr >>= 1;
                    if (nextBit) m_lfsr |= (1 << 16);
                    m_currentNoiseSample = (m_lfsr & 1) ? 1.0f : -1.0f;
                }
                lfoVal = m_currentNoiseSample;
                break;
            }

            float lfoAmpMod = 1.0f;
            if (m_ams > 0) {
                float depths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
                lfoAmpMod = 1.0f - (std::abs(lfoVal) * depths[m_ams & 3]);
            }

            float lfoPitchMod = 1.0f;
            if (m_pms > 0) {
                float depths[] = { 0.0f, 0.001f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f, 0.2f };
                lfoPitchMod = 1.0f + (lfoVal * depths[m_pms & 7]);
            }

            float out1, out2, out3, out4;
            m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

            float finalOut = 0.0f;
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

    void updateNoiseDelta(double targetRate) {
        if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
    }

    std::array<FmOperator, 4> m_operators;
    double m_hostSampleRate = 44100.0;
    int m_algorithm = 0;

    // Rate & Quality
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    // OPM LFO
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    int m_pms = 0; int m_ams = 0; int m_lfoWave = 2;

    // Noise LFSR Variables
    unsigned int m_lfsr = 0x1FFFF; // 17-bit Shift Register (Seed must be non-zero)
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;
};