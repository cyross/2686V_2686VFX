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
        for (auto& op : m_operators) op.setSampleRate(sampleRate);
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm; // 0:Serial(FM), 1:Parallel(AM)

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
    }

    void noteOff() {
        m_operators[0].noteOff();
        m_operators[1].noteOff();
    }

    bool isPlaying() const { return m_operators[0].isPlaying() || m_operators[1].isPlaying(); }

    float getSample() {
        float out1, out2;

        // OP1 (Modulator)
        m_operators[0].getSample(out1, 0.0f);

        if (m_algorithm == 0) // Serial (FM): Op1 -> Op2
        {
            m_operators[1].getSample(out2, out1); // out1 modulates Op2
            return out2;
        }
        else // Parallel (AM): Op1 + Op2
        {
            m_operators[1].getSample(out2, 0.0f);
            return (out1 + out2) * 0.5f;
        }
    }

private:
    std::array<FmOperator, 2> m_operators;
    int m_algorithm = 0;
};
