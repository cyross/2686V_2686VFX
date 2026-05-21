#pragma once

#include "./Params.h"

class FmRgAdddr
{
	enum class State { Idle, Attack, Decay, Sustain, Release };
	State state = State::Idle;

	int ar = 0;
	int d1r = 0;
	int d2r = 0;
	int d1l = 15;
	int rr = 0;
	int tl = 0;
	int ks = 0;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;
	float sustainRateDec = 0.0f;
	float susReleaseDec = 0.0f;
	float currentReleaseDec = 0.0f;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
public:
	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const FmRgAdddrParams& params);
	float noteOn(float velocity);
	void noteOff();
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void updateSampleRate(double newSampleRate);
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
};
