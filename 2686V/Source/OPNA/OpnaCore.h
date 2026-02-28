#pragma once

#include "../fm/FmCommon.h"

// ==========================================================
// OPNA (YM2608) Core
// Features: 4 Operators, 8 Algorithms, SSG-EG, Hardware LFO
// ==========================================================
class OpnaCore
{
public:
    OpnaCore() {}

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
    bool m_pm = false;
    bool m_am = false;

    float m_modWheel = 0.0f;
};
