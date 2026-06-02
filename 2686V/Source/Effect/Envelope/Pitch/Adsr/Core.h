#pragma once

#include <functional>

#include "./Params.h"
#include "../../../../Advanced/Curve/Core.h"

class PitchAdsrEnv {
	enum class State { Idle, Attack, Decay, Sustain, Release };
	State state = State::Idle;

	float ar = 0.0f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.0f;
	int stl = 0;
	int atl = 0;
	int ssl = 0;
	int rll = 0;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float phaseProgress = 0.0f;     // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float currentCents = 0.0f;      // 現在出力されているピッチ補正値 (セント)
	float releaseStartCents = 0.0f; // ノートオフされた瞬間のピッチ補正値
	float attackDelta = 0.0f;
	float decayDelta = 0.0f;
	float releaseDelta = 0.0f;

	void updateIncrements();

	// カーブモード用の変数
	int targetIndex = 0; // 0,1,2,3,4
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)

	std::array<std::function<void()>, 2> noteOnFunctions;
	std::array<std::function<void()>, 2> noteOffFunctions;
	std::array<std::array<std::function<float(float)>, 5>, 2> processFunctions;
public:
	PitchAdsrEnv();
	void prepare(int targetIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	bool isBypass() const { return bypass; }
	void setParameters(const PitchAdsrParams& params);
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	void noteOn();
	void noteOff();
	float process(float phaseDelta);
	float bypassedReleasedProcess();
	float bypassedProcess();
};
