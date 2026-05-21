#pragma once

#include <array>

#include "./Params.h"

class SsgSwEnv {
	enum class State { Idle, S1, S2, S3, S4, S5, S6 };
	State state = State::Idle;

	float m_currentRate = 0.0f;

	int steps = 4;
	bool loop = false;
	int loopTo = 0;
	int loopCount = 0;
	std::array<float, 7> r = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::array<float, 7> l = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float currentLevel = 0.0f;

	std::array<float, 7> rInc = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	std::array<float, 7> rIncLoop = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	int loopCounter = 0;

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
