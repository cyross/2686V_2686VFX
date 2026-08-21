#pragma once

#include <array>
#include <functional>

#include "./EnvSsgSw11Params.h"

class SsgSwPEnv11 {
	enum class State { Idle, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11 };
	State state = State::Idle;

	float m_currentRate = 0.0f;

	int steps = 4;
	bool loop = false;
	int loopTo = 0;
	int loopCount = 0;
	std::array<float, 12> r = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::array<int, 12> l = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float currentLevel = 0.0f;

	std::array<float, 12> rInc = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	std::array<float, 12> rIncLoop = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	int loopCounter = 0;

	void updateIncrements();
	inline bool isReached(float inc, float current, float target) const; 

	// 時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
public:
	SsgSwPEnv11();
	void prepare(int targetIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::S6; }
	bool isBypass() const { return bypass; }
	void setParameters(const SsgSwPEnv11Params& params);
	void noteOn();
	void noteOff();
	float process(float phaseDelta);
	float bypassedReleasedProcess();
	float bypassedProcess();
};
