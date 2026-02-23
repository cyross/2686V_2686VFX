#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <vector>
#include <array>
#include <algorithm>

#include "../synth/SynthParams.h"

// ==========================================================
// Shared FM Operator Class
// ==========================================================
class FmOperator
{
public:
    FmOperator() {}
    void setSampleRate(double sampleRate) { m_sampleRate = sampleRate; }
    void setParameters(const FmOpParams& params, float feedback, bool useSsgEg, bool useWaveSelect, bool useOpmEg = false, float ssgEgFreq = 1.0f);
    void noteOn(float frequency, float velocity, int noteNumber);
    void noteOff() { m_state = State::Release; }
    bool isPlaying() const { return m_state != State::Idle; }
    float getCurrentEnvelope() const { return m_currentLevel; }
    void setPitchBendRatio(float ratio) { m_pitchBendRatio = ratio; }
    void getSample(float& output, float modulator, float lfoAmp = 1.0f, float lfoPitch = 1.0f);
private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;
    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    double m_ssgPhase = 0.0;
    float m_ssgEgFreq = 1.0f;
    int m_noteNumber = 60;

    FmOpParams m_params;
    float m_feedback = 0.0f;
    bool m_useSsgEg = false;
    bool m_useWaveSelect = false;
    bool m_useOpmEg = false;

    float m_currentLevel = 0.0f;
    float m_targetLevel = 0.0f;
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;
    float m_sustainRateDec = 0.0f;
    float m_fb1 = 0.0f; float m_fb2 = 0.0f;

    float m_pitchBendRatio = 1.0f;

    float calcWaveform(double phase, int wave);
    float getSsgEnvelopeLevel(double p);
    void updateEnvelopeState();
    void updateIncrementsWithKeyScale();
};
