#pragma once

#include "./Params.h"

class SsgSwEnv {
	enum class State { Idle, S1, S2, S3, S4, S5, S6 };
	State state = State::Idle;

	float m_currentRate = 0.0f;

	int steps = 4;
	bool loop = false;
	int loopTo = 0;
	int loopCount = 0;
	float stl = 0.0f;
	float r1 = 0.0f;
	float l1 = 0.0f;
	float r2 = 0.0f;
	float l2 = 0.0f;
	float r3 = 0.0f;
	float l3 = 0.0f;
	float r4 = 0.0f;
	float l4 = 0.0f;
	float r5 = 0.0f;
	float l5 = 0.0f;
	float r6 = 0.0f;
	float l6 = 0.0f;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float currentLevel = 0.0f;

	float r0Inc = 0.0f;
	float r1Inc = 0.0f;
	float r2Inc = 0.0f;
	float r3Inc = 0.0f;
	float r4Inc = 0.0f;
	float r5Inc = 0.0f;
	float r6Inc = 0.0f;

	void updateIncrements();
	inline bool isReached(float inc, float current, float target) const; 
public:
	void prepare(double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const SsgSwEnvParams& params);
	void noteOn();
	void noteOff();
	float process();
	void  bypassedReleasedProcess();
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::S6; }
	bool isBypassed() const { return bypass; }
};
