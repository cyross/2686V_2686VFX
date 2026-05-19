#pragma once

#include "./Params.h"

struct Opzx7RealAdssr
{
	float ar = 0.0f;
	float d1r = 0.0f;
	float d2r = 0.0f;
	float d1l = 1.0f;
	float rr = 0.0f;
	float tl = 0.0f;
};

struct Opzx7RgAdssr
{
	int ar = 0;
	int d1r = 0;
	int d2r = 0;
	int d1l = 15;
	int rr = 0;
	int tl = 0;
};

class Opzx7Adddr
{
	enum class State { Idle, Attack, Decay, Sustain, Release };
	State state = State::Idle;

	bool rgEnable = false;

	Opzx7RealAdssrParams real;
	Opzx7RgAdssrParams rg;

	int ks = 0;
	bool sus = false;
	bool xof = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float sustainRateDec = 0.0f;
	float susReleaseDec = 0.0f;
	float releaseDec = 0.0f;
	float currentReleaseDec = 0.0f;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
	inline float getReleaseDec() const
	{
		if (sus) {
			return 0.0f;
		}
		else {
			return releaseDec;
		}
	}
public:
	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const Opzx7AdddrParams& params);
	float noteOn(float velocity);
	void noteOff();
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void updateSampleRate(double newSampleRate);
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
};
