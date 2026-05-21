#pragma once

#include <array>

#include "./Params.h"

class OplAdsr
{
	enum class State { Idle, Attack, Decay, Sustain, Release };
	State state = State::Idle;

	static const std::array<float, 4> dbPerOcts;

	int ar = 0;
	int dr = 0;
	int sl = 15;
	int rr = 0;
	int tl = 0;
	int ksr = 0;
	int ksl = 0;
	bool sus = false;
	bool egType = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;
	float sustainRateDec = 0.0f;
	float susReleaseDec = 0.0f;
	float currentReleaseDec = 0.0f;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
	float getReleaseDec() const;
public:
	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const OplAdsrParams& params);
	float noteOn(float velocity, int noteNumber);
	void noteOff();
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void updateSampleRate(double newSampleRate);
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
};
