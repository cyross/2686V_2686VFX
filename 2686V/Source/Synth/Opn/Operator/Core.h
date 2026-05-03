#pragma once

#include <array>

#include "./Params.h"

class OpnOperator
{
public:
	OpnOperator() {}

	OpnOpParams m_params;

	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f) override;

    static const std::array<float, 8> dtScales; // DT(DT1)値のリスト
    static const std::array<float, 4> dt2Scales; // DT2値のリスト

    using SsgWaveCalculator = float(*)(double p);

    static const std::array<SsgWaveCalculator, 16> ssgWaveStrategies;

    void setSampleRate(double sampleRate) { sampleRate = sampleRate; }
    void setHostSampleRate(double hostRate) { hostSampleRate = hostRate; }
    void setParameters(const FmOpParams& params, float feedback);
    void noteOn(float frequency, float velocity, int noteNumber) {};
    void noteOff();
    bool isPlaying() const { return state != State::Idle; }
    float getCurrentEnvelope() const { return currentLevel; }
    void setPitchBendRatio(float ratio) { pitchBendRatio = ratio; }
    void getSample(float& output, float modulator, float lfoAmp, float lfoPitch) {};
    void getSample(float& output, float modulator, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f) {};
    void setExternalFeedbackMode(bool isExternal) { isExternalFeedback = isExternal; }
    void pushFeedback(float fbValue) { fb2 = fb1; fb1 = fbValue; }
    // OPZX3 の外部 PCM データ用
    void setPcmBuffer(const std::vector<float>* pcmData) { pcmBuffer = pcmData; }
    float calcWaveform(double phase, int wave);
    void updateIncrementsWithKeyScale();

    double hostSampleRate = 44100.0;
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State state = State::Idle;
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseDelta = 0.0f;
    double ssgPhase = 0.0;
    float ssgEgFreq = 1.0f;
    int noteNumber = 60;

    float feedback = 0.0f;
    bool isExternalFeedback = false;

    float currentLevel = 0.0f;
    float targetLevel = 0.0f;
    float attackInc = 0.0f;
    float decayDec = 0.0f;
    float releaseDec = 0.0f;
    float sustainRateDec = 0.0f;
    float fb1 = 0.0f;
    float fb2 = 0.0f;

    float pitchBendRatio = 1.0f;

    float susReleaseDec = 0.0f;
    float currentReleaseDec = 0.0f;

    // OPZX3 の外部 PCM データ用
    const std::vector<float>* pcmBuffer = nullptr;

    void updateEnvelopeState();
};
