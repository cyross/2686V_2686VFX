#pragma once

#include <array>
#include <functional>

#include "./Params.h"
#include "../../../../Advanced/Curve/Core.h"

class AmpAdsrEnv {
	enum class State { Idle, Attack, Decay, Sustain, Release, Size };
	State state = State::Idle;

	float stl = 0.0f;
	float ar = 0.01f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.2f;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;

	// カーブモード用の変数
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)

	std::array<std::function<float()>, 2> noteOnFunctions;
	std::array<std::function<void()>, 2> noteOffFunctions;
	std::array<std::array<std::function<float(float)>, 5>, 2> processFunctions;
	std::array<std::function<float()>, 2> bypassedReleasedProcessFunctions;
	std::array<std::function<float()>, 2> bypassedProcessFunctions;

	void updateIncrements();
public:
	AmpAdsrEnv();
	void prepare(double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isBypass() const { return bypass; }
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void setParameters(const AmpAdsrParams& params);
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	float noteOn();
	void noteOff();
	float process(float currentLevel);
	float bypassedReleasedProcess();
	float bypassedProcess();
};
