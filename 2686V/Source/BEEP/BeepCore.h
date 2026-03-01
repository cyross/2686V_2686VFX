#pragma once

#include "../synth/SynthParams.h"
#include <cmath>

class BeepCore
{
public:
    void prepare(double sampleRate);
    void setParameters(const SynthParams& params);
    void noteOn(float freq, float velocity);
    void noteOff();
    bool isPlaying() const;
    void setPitchBend(int pitchWheelValue);
    float getSample();
    void setModulationWheel(int wheelValue);
    void setPitchBendRatio(float ratio);

private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;

    double m_sampleRate = 44100.0;
    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_baseFreq = 440.0f;
    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    float m_currentLevel = 0.0f;
    float m_targetLevel = 0.0f;

    // ADSR increments
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;

    // Params
    float m_level = 1.0f;
    bool m_bypassAdsr = false;
    SimpleAdsr m_adsr;

    bool m_fixedMode = false;
    float m_fixedFreq = 2000.0f;

    void updateEnvelope();
};
