#pragma once

#include <functional>
#include <array>

#include "./Params.h"
#include "../../../../Advanced/Curve/Core.h"

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
	enum class State { Idle, Attack, Decay, Sustain, Release, Size };
	State state = State::Idle;

	bool rgEnable = false;

	Opzx7RealAdssrParams real;
	Opzx7RgAdssrParams rg;
	Opzx7RgAdssrParams rgMax;

	std::array<float, 64> timeInSecondsLut;
	std::array<float, 64> attcckTimeInSecondsLut;

	int ks = 0;
	bool sus = false;
	bool xof = false;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float totalLevel = 0.0f;

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float sustainRateDec = 0.0f;
	float susReleaseDec = 0.0f;
	float releaseDec = 0.0f;
	float currentReleaseDec = 0.0f;
	float m_attackStartLevel = 0.0f; // アタック開始時のレベル

	// rrが無限大のとき、ストッパーの役目を果たす
	int releaseCounter = 0;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
	inline float getReleaseDec() const
	{
		if (sus) {
			return susReleaseDec;
		}
		else {
			return releaseDec;
		}
	}

	int m_noteNumber = 60; // C3

	// カーブモード用の変数
	int positionIndex = 1; // 1,2,3,4
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)

	std::array<std::function<void(const Opzx7AdddrParams&)>, 2> setParameterFunctions;
	std::array<std::function<float(float)>, 2> noteOnFunctions;
	std::array<std::function<void(int)>, 2> updateIncrementsWithKeyScaleFunctions;
	std::array<std::array<std::function<float(float)>, 5>, 2> updateEnvelopeStateFunctions;
public:
	Opzx7Adddr();
	void prepare(int posIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isBypass() const { return bypass; }
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	void setParameters(const Opzx7AdddrParams& params);
	float noteOn(float velocity);
	void noteOff();
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
	void setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl);
};
