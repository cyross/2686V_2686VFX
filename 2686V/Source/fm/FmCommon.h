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

    FmOpParams m_params;

    void setSampleRate(double sampleRate) { m_sampleRate = sampleRate; }
    void setParameters(const FmOpParams& params, float feedback, bool useSsgEg, bool useWaveSelect, bool useOpmEg = false, float ssgEgFreq = 1.0f);
    void noteOn(float frequency, float velocity, int noteNumber);
    void noteOff() { m_state = State::Release; }
    bool isPlaying() const { return m_state != State::Idle; }
    float getCurrentEnvelope() const { return m_currentLevel; }
    void setPitchBendRatio(float ratio) { m_pitchBendRatio = ratio; }
    void getSample(float& output, float modulator, float lfoAmp, float lfoPitch);
    void getSample(float& output, float modulator, float lfoVal, bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f);
    void setExternalFeedbackMode(bool isExternal) { m_isExternalFeedback = isExternal; }
    void pushFeedback(float fbValue) { m_fb2 = m_fb1; m_fb1 = fbValue; }
    // OPZX3 の外部 PCM データ用
    void setPcmBuffer(const std::vector<float>* pcmData) { m_pcmBuffer = pcmData; }
private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;
    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    double m_ssgPhase = 0.0;
    float m_ssgEgFreq = 1.0f;
    int m_noteNumber = 60;

    float m_feedback = 0.0f;
    bool m_isExternalFeedback = false;
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

    // OPZX3 の外部 PCM データ用
    const std::vector<float>* m_pcmBuffer = nullptr;

    float calcWaveform(double phase, int wave, bool isOpl);
    float getSsgEnvelopeLevel(double p);
    void updateEnvelopeState();
    void updateIncrementsWithKeyScale();
};
