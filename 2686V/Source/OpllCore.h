#pragma once
#include "FmCommon.h"

// ==========================================================
// OPLL (YM2413) Core
// Features: 2 Operators, User Patch Only (Full Edit)
// ==========================================================
class OpllCore
{
public:
    OpllCore() {}
    void prepare(double sampleRate) {
        for (auto& op : m_operators) op.setSampleRate(sampleRate);
    }

    void setParameters(const SynthParams& params) {
        for (int i = 0; i < 2; ++i)
        {
            FmOpParams p = params.fmOp[i];
            p.keyScale = (params.fmOp[i].keyScale > 0) ? 3 : 0;
            float fb = (i == 0) ? params.feedback : 0.0f;
            m_operators[i].setParameters(p, fb, false, true);
        }
    }

    void noteOn(float freq, float velocity) {
        int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
        m_operators[0].noteOn(freq, velocity, noteNum);
        m_operators[1].noteOn(freq, velocity, noteNum);
    }
    void noteOff() { m_operators[0].noteOff(); m_operators[1].noteOff(); }
    bool isPlaying() const { return m_operators[0].isPlaying(); }

    float getSample() {
        // --- OPLL LFO Generation ---
        // AM: ~3.7 Hz, VIB: ~6.4 Hz

        m_amPhase += (3.7 / m_sampleRate);
        if (m_amPhase >= 1.0) m_amPhase -= 1.0;

        m_vibPhase += (6.4 / m_sampleRate);
        if (m_vibPhase >= 1.0) m_vibPhase -= 1.0;

        // OPLL LFO Waveforms (Simplified Triangle)
        float amVal = (m_amPhase < 0.5) ? (float)m_amPhase * 2.0f : (float)(1.0 - m_amPhase) * 2.0f;
        float vibVal = (m_vibPhase < 0.5) ? (float)m_vibPhase * 2.0f : (float)(1.0 - m_vibPhase) * 2.0f;

        // Depth (Approximation)
        // AM: Deep depth (4.8dB?) -> 0.0 ~ 1.0 scale mapping
        // 1.0 (No Atten) to 0.5 (Max Atten) approx
        float lfoAmp = 1.0f - (amVal * 0.5f);

        // VIB: 14 cents -> Pitch scale 1.0 ~ 1.008
        // 1.0 +/- depth
        float lfoPitch = 1.0f + ((vibVal - 0.5f) * 0.01f);

        float out1, out2;
        m_operators[0].getSample(out1, 0.0f, lfoAmp, lfoPitch);
        m_operators[1].getSample(out2, out1, lfoAmp, lfoPitch);
        return out2;
    }

private:
    std::array<FmOperator, 2> m_operators;
    double m_sampleRate = 44100.0;

    // LFO States
    double m_amPhase = 0.0;
    double m_vibPhase = 0.0;
};
