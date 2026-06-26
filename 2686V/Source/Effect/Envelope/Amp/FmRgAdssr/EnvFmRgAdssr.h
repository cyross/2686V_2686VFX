#pragma once

#include <functional>

#include "./EnvFmRgAdssrParams.h"
#include "../../../KeyScale/Opn/KSOpn.h"
#include "../../../../Advanced/Curve/AdvancedCurve.h"

class FmRgAdssr
{
	enum class State { Idle, Attack, Decay, Sustain, Release, Size };
	State state = State::Idle;

	std::array<float, 64> timeInSecondsLut;
	std::array<float, 64> attcckTimeInSecondsLut;

	int ar = 0;
	int dr = 0;
	int sr = 0;
	int sl = 15;
	int rr = 0;
	int tl = 0;

	KSOpn m_ksOPN;

	int arMax = 0;
	int drMax = 0;
	int srMax = 0;
	int slMax = 0;
	int rrMax = 0;
	int tlMax = 0;

	bool xof = false;
	bool kor = false;
	bool bypass = false;

	int m_noteNumber = 60; // C3

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float totalLevel = 0.0f;

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;
	float sustainRateDec = 0.0f;
	float m_attackStartLevel = 0.0f; // アタック開始時のレベル
	float releaseTimeInc = 0.0f;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	// rrが無限大のとき、ストッパーの役目を果たす
	int releaseCounter = 0;

	// カーブモード用の変数
	int positionIndex = 1; // 1,2,3,4
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)
public:
	FmRgAdssr();
	void prepare(int posIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isBypass() const { return bypass; }
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	void setParameters(const FmRgAdssrParams& params);
	float noteOn(float velocity);
	void noteOff();
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
	void setParamMax(int ar, int dr, int sr, int sl, int rr, int tl);
};
