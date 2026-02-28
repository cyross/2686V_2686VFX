#pragma once

#include "../fm/FmCommon.h"

// ==========================================================
// OPL (YM3526/3812) Core
// Features: 2 Operators, 2 Algorithms (FM/AM), Wave Select
// ==========================================================
class OplCore
{
public:
    OplCore() {}

    void prepare(double sampleRate);
    void setParameters(const SynthParams& params);
    void noteOn(float freq, float velocity);
    void noteOff();
    bool isPlaying() const;
    void setPitchBend(int pitchWheelValue);
    void setModulationWheel(int wheelValue);
    float getSample();
private:
    std::array<FmOperator, 2> m_operators;
    std::array<bool, 2> m_opMask{ false, false };

    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;
    double m_amPhase = 0.0;
    double m_vibPhase = 0.0;

    float m_modWheel = 0.0f;
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
};
