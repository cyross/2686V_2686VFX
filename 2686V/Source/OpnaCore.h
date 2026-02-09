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
        if (sampleRate > 0.0) m_sampleRate = sampleRate;

        for (auto& op : m_operators) op.setSampleRate(m_sampleRate);
        m_lfoPhase = 0.0;
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;
        m_lfoFreq = params.lfoFreq;
        m_pms = params.pms;
        m_ams = params.ams;

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
    }

    void noteOff() { for (auto& op : m_operators) op.noteOff(); }
    bool isPlaying() const { return m_operators[0].isPlaying(); }

    float getSample() {
        // --- LFO Calculation (Shared) ---
        double lfoInc = m_lfoFreq / m_sampleRate;
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

        float lfoValue = 0.0f;
        if (m_lfoPhase < 0.25)      lfoValue = (float)(m_lfoPhase * 4.0);
        else if (m_lfoPhase < 0.75) lfoValue = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
        else                        lfoValue = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

        // AMS (Amp Mod)
        float lfoAmpMod = 1.0f;
        if (m_ams > 0) {
            float depths[] = { 0.0f, 0.1f, 0.3f, 0.7f };
            lfoAmpMod = 1.0f - (std::abs(lfoValue) * depths[m_ams & 3]);
        }

        // PMS (Pitch Mod)
        float lfoPitchMod = 1.0f;
        if (m_pms > 0) {
            float depths[] = { 0.0f, 0.001f, 0.002f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f };
            lfoPitchMod = 1.0f + (lfoValue * depths[m_pms & 7]);
        }

        // --- 4-Op Routing ---
        float out1, out2, out3, out4;
        m_operators[0].getSample(out1, 0.0f, lfoAmpMod, lfoPitchMod);

        switch (m_algorithm) {
        case 0: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); return out4;
        case 1: m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);    m_operators[2].getSample(out3, out1 + out2, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); return out4;
        case 2: m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod);    m_operators[2].getSample(out3, out2, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3 + out1, lfoAmpMod, lfoPitchMod); return out4;
        case 3: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out3, lfoAmpMod, lfoPitchMod); return out2 + out4;
        case 4: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); return out2 + out3 + out4;
        case 5: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, out1, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, out1, lfoAmpMod, lfoPitchMod); return out2 + out3 + out4;
        case 6: m_operators[1].getSample(out2, out1, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); return out2 + out3 + out4; 
        default: // Algo 7
            m_operators[1].getSample(out2, 0, lfoAmpMod, lfoPitchMod); m_operators[2].getSample(out3, 0, lfoAmpMod, lfoPitchMod); m_operators[3].getSample(out4, 0, lfoAmpMod, lfoPitchMod); return out1 + out2 + out3 + out4;
        }
    }

private:
    std::array<FmOperator, 4> m_operators;
    double m_sampleRate = 44100.0;
    int m_algorithm = 0;

    // LFO State
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    int m_pms = 0;
    int m_ams = 0;
};
