#pragma once

#include "./Params.h"

class AmpAdsrEnv {
	enum class State { Idle, Attack, Decay, Sustain, Release };
	State state = State::Idle;

	float ar = 0.01f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.2f;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;

	void updateIncrements();
public:
	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const AmpAdsrParams& params);
	void noteOn();
	void noteOff();
	float process(float currentLevel);
	float bypassedReleasedProcess();
	float bypassedProcess();
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	bool isBypassed() const { return bypass; }
	void updateSampleRate(double newSampleRate);
};
