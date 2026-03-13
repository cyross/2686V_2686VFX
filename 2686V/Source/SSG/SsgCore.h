#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <cstdlib>

#include "../synth/SynthParams.h"
#include "../synth/SynthCore.h"

class SsgCore : SynthCore
{
public:
    SsgCore();
    void prepare(double sampleRate);
    void setSampleRate(double sampleRate);
    void setParameters(const SynthParams& params);
    void noteOn(float frequency);
    void noteOff();
    bool isPlaying() const;
    void setPitchBend(int pitchWheelValue);
    void setModulationWheel(int wheelValue);
    void setPitchBendRatio(float ratio);
    float getSample();
private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;

    float m_level = 1.0f;
    float m_noiseLevel = 0.0f;
    float m_mix = 0.5;
    int m_waveform = 0; // 0:Pulse, 1:Triangle

    SimpleAdsr m_adsr;
    bool m_adsrBypass = false;

    // HW Env Params
    bool m_useHwEnv = false;
    int m_envShape = 0;
    float m_envFreq = 1.0f;
    double m_hwEnvPhase = 0.0;

    // Duty Params
    int m_dutyMode = 0;
    int m_dutyPreset = 0;
    float m_dutyVar = 0.5f;
    bool m_dutyInvert = false;

	// Triangle Params
    bool m_triKeyTrack = true;
    float m_triPeak = 0.5f; // Peak Position
    float m_triFreq = 440.0f;

    // Noise LFSR Params
    unsigned int m_lfsr = 0x1FFFF; // 17-bit Shift Register (Seed must be non-zero)
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_baseNoiseFreq = 12000.0f; // Slider Value
    float m_targetNoiseFreq = 12000.0f; // Active Frequency
    bool m_noiseOnNote = false;

    // Rate / Quality Params
    int m_rateIndex = 1; // Default 55.5k
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 15.0f; // Default 4bit
    float m_currentFrequency = 440.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;

    void updateIncrements();
    void updateNoiseFrequency();
    void updatePhaseDelta();
};
