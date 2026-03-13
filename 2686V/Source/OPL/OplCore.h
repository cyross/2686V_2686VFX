#pragma once

#include "../fm/FmCore.h"
#include "../fm/FmCommon.h"

// ==========================================================
// OPL (YM3526/3812) Core
// Features: 2 Operators, 2 Algorithms (FM/AM), Wave Select
// ==========================================================
class OplCore : public FmCore
{
public:
    OplCore() : FmCore() {}

    void prepare(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
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

    std::array<double, 2> m_amPhases{ 0.0, 0.0 };
    std::array<double, 2> m_vibPhases{ 0.0, 0.0 };
};
