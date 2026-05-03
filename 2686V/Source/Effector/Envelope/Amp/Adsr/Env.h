#pragma once

#include "./Params.h"

class AmpAdsrEnv
{
	enum class State { Idle, Attack, Decay, Sustain, Release };

	float ar;
	float dr;
	float sl;
	float rr;
	float tl;
	double sampleRate = 44100.0;
	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;
	State state = State::Idle;
	bool bypass = false;
public:
	AmpAdsrEnv() : AmpAdsrEnv(0.03f, 0.0f, 1.0f, 0.0f, 1.0f) {}
	AmpAdsrEnv(float ar, float dr, float sl, float rr, float tl);
	void setParameters(const AmpAdsrEnvParams& params, bool bypass);
	void updateIncrements();
	void updateIncrements(double newSampleRate);
	void noteOn() { state = State::Attack; }
	void noteOff() { state = State::Release; }
	bool isPlaying() const { return state != State::Idle; }
	bool isReleasing() const { return state == State::Release; }
	void idle() { state = State::Idle; }
	float process(float currentLevel);
	bool isBypassed() const { return bypass; }
};
