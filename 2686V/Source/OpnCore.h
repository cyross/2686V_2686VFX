#pragma once
#include "FmCommon.h"

// ==========================================================
// OPN (YM2203) Core
// Features: 4 Operators, 8 Algorithms, No SSG-EG, No HW LFO
// ==========================================================
class OpnCore
{
public:
    OpnCore() {}

    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_sampleRate = sampleRate;

        for (auto& op : m_operators) op.setSampleRate(m_sampleRate);
    }

    void setParameters(const SynthParams& params) {
        m_algorithm = params.algorithm;
        for (int i = 0; i < 4; ++i) {
            float fb = (i == 0) ? params.feedback : 0.0f;
            // OPN: SSG-EG=False, WaveSelect=False
            m_operators[i].setParameters(params.fmOp[i], fb, false, false);
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
        float out1, out2, out3, out4;

        // No LFO modulation (1.0f)
        m_operators[0].getSample(out1, 0.0f);

        switch (m_algorithm) {
        case 0: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, out2); m_operators[3].getSample(out4, out3); return out4;
        case 1: m_operators[1].getSample(out2, 0);    m_operators[2].getSample(out3, out1 + out2); m_operators[3].getSample(out4, out3); return out4;
        case 2: m_operators[1].getSample(out2, 0);    m_operators[2].getSample(out3, out2); m_operators[3].getSample(out4, out3 + out1); return out4;
        case 3: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, out3); return out2 + out4;
        case 4: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, 0); return out2 + out3 + out4;
        case 5: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, out1); m_operators[3].getSample(out4, out1); return out2 + out3 + out4;
        case 6: m_operators[1].getSample(out2, out1); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, 0); return out2 + out3 + out4;
        default: // Algo 7
            m_operators[1].getSample(out2, 0); m_operators[2].getSample(out3, 0); m_operators[3].getSample(out4, 0); return out1 + out2 + out3 + out4;
        }
    }

private:
    std::array<FmOperator, 4> m_operators;
    int m_algorithm = 0;
    double m_sampleRate = 44100.0;
};
