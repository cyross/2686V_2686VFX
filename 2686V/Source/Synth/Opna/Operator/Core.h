#pragma once

#include <array>

#include "../../../Generator/Fm/Fix/Fix.h"
#include "../../../Effector/Envelope/Fm/Opn/Env.h"
#include "../../../Effector/Envelope/Amp/Adssr/Env.h"
#include "../../../Effector/Envelope/Ssg/Env.h"
#include "../../../Effector/Lfo/Opna/Lfo.h"
#include "../../../Effector/Lfo/N88/Lfo.h"
#include "./Params.h"

class OpnaOperator
{
public:
	OpnaOperator() {}

	AmpAdssrEnv ampEnv;
	OpnEnv n88Env;

    OpnaLfo* opnaLfo;
	N88Lfo n88Lfo;
 
    bool am = false;

	// Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
	int waveSelect = 0;

	SsgEnv ssgEnv;
	FixMode fixMode;

	void prepare(OpnaLfo* pOpnaLfo);
    void setSampleRate(double sampleRate) { this->sampleRate = sampleRate; }
    void setHostSampleRate(double hostRate) { hostSampleRate = hostRate; }
    void setParameters(const OpnaOpParams& params, float feedback, N88LfoParams lfoParams);
	void noteOn(float frequency, float velocity, int noteNumber);
    void noteOff();
    bool isPlaying() const { return state != State::Idle; }
    float getCurrentEnvelope() const { return currentLevel; }
    void setPitchBendRatio(float ratio) { pitchBendRatio = ratio; }
    void getSample(float& output, float modulator, float modWheel = 0.0f);
    void setExternalFeedbackMode(bool isExternal) { isExternalFeedback = isExternal; }
    void pushFeedback(float fbValue) { fb2 = fb1; fb1 = fbValue; }

    static const std::array<float, 8> dtScales; // DT(DT1)値のリスト

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
