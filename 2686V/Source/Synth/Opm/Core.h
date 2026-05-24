#pragma once

#include <random>

#include "../../Core/Fm/FmCore.h"
#include "../../Generator/Noise/Lfsr/Core.h"
#include "../../Effect/Lfo/opm/Core.h"
#include "../../Advanced/Curve/Core.h"

#include "./Operator/Core.h"

class OpmCore : public FmCore
{
public:
    OpmCore() : FmCore() {}
    void prepare(double sampleRate) override;
    void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setCurveCore(CurveCore* p_curveCore);
private:
    std::array<OpmOperator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };
    LfsrNoiseGen m_noiseGen;
    OpmLfoCore m_lfo;

    double m_hostSampleRate = 44100.0;
    int m_algorithm = 0;

    // Rate & Quality
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    float m_modWheel = 0.0f;

    int m_pan = 0;
};