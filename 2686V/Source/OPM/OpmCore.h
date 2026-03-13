#pragma once

#include <random>

#include "../fm/FmCore.h"
#include "../fm/FmCommon.h"

class OpmCore : FmCore
{
public:
    OpmCore() : FmCore() {}
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
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    // OPM LFO
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    int m_pms = 0; int m_ams = 0; int m_lfoWave = 2;
    bool m_pm = false;
    bool m_am = false;
    int m_pmd = 0;
    int m_amd = 0;

    // Noise LFSR Variables
    unsigned int m_lfsr = 0x1FFFF; // 17-bit Shift Register (Seed must be non-zero)
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;
    float m_amSmooth = 0.0f;
    float m_amSmoothRate = 0.005f;

    float m_modWheel = 0.0f;

    void updateNoiseDelta(double targetRate);
};