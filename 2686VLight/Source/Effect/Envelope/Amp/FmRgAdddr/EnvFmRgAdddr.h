#pragma once

#include <functional>

#include "./EnvFmRgAdddrParams.h"
#include "../../../KeyScale/Opn/KSOpn.h"
#include "../../../KeyScale/Opp/KSOpp.h"

class FmRgAdddr
{
	enum class State { Idle, Attack, Decay, Sustain, Release, Size };
	State state = State::Idle;

	std::array<float, 64> timeInSecondsLut;
	std::array<float, 64> attcckTimeInSecondsLut;

	int ar = 0;
	int d1r = 0;
	int d2r = 0;
	int d1l = 15;
	int rr = 0;
	int tl = 0;
	FmRgAdddrKeyScaleMode m_ksMode = FmRgAdddrKeyScaleMode::OPM;
	KSOpn m_ksOPM;
	KSOpp m_ksOPP;

	// レジスタ幅の既定値。setParamMax() より先に setParameters() が来ても
	// 0 除算 → LUT の範囲外参照にならないよう、実機の幅を入れてある。
	// 実際の値はオペレータの prepare() が setParamMax() で上書きする。
	int arMax = 31;
	int d1rMax = 31;
	int d2rMax = 31;
	int d1lMax = 15;
	int rrMax = 15;
	int tlMax = 127;

	bool xof = false;
	bool kor = false;
	bool bypass = false;

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

	int m_noteNumber = 60; // C3

	// 時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)

	int calcRateScaling() const;
	float calcLevelScalingDb() const;
public:
	FmRgAdddr();
	void prepare(int posIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isBypass() const { return bypass; }
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void setParameters(const FmRgAdddrParams& params);
	float noteOn(float velocity, int noteNumber);
	void noteOff();
	void updateIncrementsWithKeyScale(int noteNumber);
	float updateEnvelopeState(float currentLevel);
	void setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl);
	void bypassedReleasedProcess();
};
