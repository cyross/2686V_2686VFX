#pragma once

#include "./Params.h"

class SsgSwEnv {
	enum class State { Idle, R0, R1, R2, R3, R4, R5, R6 };
	State state = State::Idle;

	float m_currentRate = 0.0f;

	int steps = 4;
	float stl = 0.0f;
	float r0 = 0.0f;
	float l0 = 0.0f;
	float r1 = 0.0f;
	float l1 = 0.0f;
	float r2 = 0.0f;
	float l2 = 0.0f;
	float r3 = 0.0f;
	float l3 = 0.0f;
	float r4 = 0.0f;
	float l4 = 0.0f;
	float r5 = 0.0f;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float r0Inc = 0.0f;
	float r1Inc = 0.0f;
	float r2Inc = 0.0f;
	float r3Inc = 0.0f;
	float r4Inc = 0.0f;
	float r5Inc = 0.0f;
	float r6Inc = 0.0f;

	void updateIncrements();
public:
	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const SsgSwEnvParams& params);
	float noteOn();
	void noteOff();
	float process(float currentLevel);
	float bypassedReleasedProcess();
	float bypassedProcess();
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::R5; }
	bool isBypassed() const { return bypass; }
	void updateSampleRate(double newSampleRate);
};
