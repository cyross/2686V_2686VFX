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

    double m_hostSampleRate = 44100.0;

    FmOpParams m_params;

    static const std::array<float, 8> dtScales; // DT(DT1)値のリスト
    static const std::array<float, 4> dt2Scales; // DT2値のリスト

    using SsgWaveCalculator = float(*)(double p);

    static const std::array<SsgWaveCalculator, 16> ssgWaveStrategies;

    void virtual setSampleRate(double sampleRate) { m_sampleRate = sampleRate; }
    void virtual setHostSampleRate(double hostRate) { m_hostSampleRate = hostRate; }
    void virtual setParameters(const FmOpParams& params, float feedback);
    void virtual noteOn(float frequency, float velocity, int noteNumber) {};
    void virtual noteOff();
    bool virtual isPlaying() const { return m_state != State::Idle; }
    float virtual getCurrentEnvelope() const { return m_currentLevel; }
    void virtual setPitchBendRatio(float ratio) { m_pitchBendRatio = ratio; }
    void virtual getSample(float& output, float modulator, float lfoAmp, float lfoPitch) {};
    void virtual getSample(float& output, float modulator, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f) {};
    void virtual setExternalFeedbackMode(bool isExternal) { m_isExternalFeedback = isExternal; }
    void virtual pushFeedback(float fbValue) { m_fb2 = m_fb1; m_fb1 = fbValue; }
    // OPZX3 の外部 PCM データ用
    void virtual setPcmBuffer(const std::vector<float>* pcmData) { m_pcmBuffer = pcmData; }
    float virtual calcWaveform(double phase, int wave);
    void virtual updateIncrementsWithKeyScale();
protected:
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

    float m_currentLevel = 0.0f;
    float m_targetLevel = 0.0f;
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;
    float m_sustainRateDec = 0.0f;
    float m_fb1 = 0.0f; float m_fb2 = 0.0f;

    float m_pitchBendRatio = 1.0f;

    float m_susReleaseDec = 0.0f;
    float m_currentReleaseDec = 0.0f;

    // LFO Sync Delay とカウンター
    float m_lfoSyncDelay = 0.0f;
    float m_lfoDelayCounter = 0.0f;
    uint32_t m_lfoCycleCount = 0;
    double m_lfoPhase = 0.0;
    float  m_amSmooth = 0.0f;

    // OPZX3 の外部 PCM データ用
    const std::vector<float>* m_pcmBuffer = nullptr;

    void updateEnvelopeState();
};
