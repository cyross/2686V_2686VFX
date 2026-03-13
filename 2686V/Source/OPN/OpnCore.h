#pragma once

#include "../fm/FmCore.h"
#include "../fm/FmCommon.h"

// ==========================================================
// OPN (YM2203) Core
// Features: 4 Operators, 8 Algorithms, No SSG-EG, No HW LFO
// ==========================================================
class OpnCore : public FmCore
{
public:
    OpnCore() : FmCore() {}

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
    std::array<FmOperator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };

    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    int m_lfoWave = 0; // 0:Sine, 1:Saw, 2:Square, 3:Tri, 4:Noise
    unsigned int m_lfsr = 0x1FFFF;
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;
    float m_amSmooth = 0.0f;
    float m_amSmoothRate = 0.005f;

    float m_modWheel = 0.0f;

    // Software LFO for Mod Wheel
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f; // Default ~5Hz

    void updateNoiseDelta(double targetRate) {
        if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
    }
};
