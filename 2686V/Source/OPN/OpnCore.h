#pragma once

#include "../fm/FmCommon.h"

// ==========================================================
// OPN (YM2203) Core
// Features: 4 Operators, 8 Algorithms, No SSG-EG, No HW LFO
// ==========================================================
class OpnCore
{
public:
    OpnCore() {}

    void prepare(double sampleRate);
    void setParameters(const SynthParams& params);
    void noteOn(float freq, float velocity);
    void noteOff();
    bool isPlaying() const;
    void setPitchBend(int pitchWheelValue);
    void setModulationWheel(int wheelValue);
    float getSample();
private:
    std::array<FmOperator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };

    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    float m_modWheel = 0.0f;

    // Software LFO for Mod Wheel
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f; // Default ~5Hz
};
