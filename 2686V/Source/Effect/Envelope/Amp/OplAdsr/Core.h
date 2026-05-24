#pragma once

#include <array>

#include <functional>

#include "./Params.h"
#include "../../../../Advanced/Curve/Core.h"

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

	float totalLevel = 0.0f;
	float sustainRate = 0.0f;

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;
	float sustainRateDec = 0.0f;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
	float getReleaseDec() const;

	// カーブモード用の変数
	int targetIndex = 1; // OPL: 1,2 OPL3: 1,2,3,4
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)

	std::array<std::function<void(const OplAdsrParams&)>, 2> setParameterFunctions;
	std::array<std::function<float(float, int)>, 2> noteOnFunctions;
	std::array<std::function<void()>, 2> noteOffFunctions;
	std::array<std::function<void(int)>, 2> updateIncrementsWithKeyScaleFunctions;
	std::array<std::function<float(float)>, 2> updateEnvelopeStateFunctions;
public:
	OplAdsr();
	void prepare(int targetIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	void setParameters(const OplAdsrParams& params);
	float noteOn(float velocity, int noteNumber);
	void noteOff();
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
	void setParametersLinear(const OplAdsrParams& params);
	float noteOnLinear(float velocity, int noteNumber);
	void noteOffLinear();
	void updateIncrementsWithKeyScaleLinear(int noteNumber);
	float updateEnvelopeStateLinear(float currentLevel);
	void setParametersCurve(const OplAdsrParams& params);
	float noteOnCurve(float velocity, int noteNumber);
	void noteOffCurve();
	void updateIncrementsWithKeyScaleCurve(int noteNumber);
	float updateEnvelopeStateCurve(float currentLevel);
};
