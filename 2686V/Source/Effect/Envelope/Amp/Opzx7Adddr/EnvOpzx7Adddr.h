#pragma once

#include <functional>
#include <array>

#include "./EnvOpzx7AdddrParams.h"
#include "../../../../Advanced/Curve/AdvancedCurve.h"

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
	State m_state = State::Idle;

	bool m_rgEnable = false;

	Opzx7RealAdssrParams m_real;
	Opzx7RgAdssrParams m_rg;
	Opzx7RgAdssrParams m_rgMax;

	static const std::array<float, 4> m_dbPerOctsMA7; // KSLの減衰カーブ定義
	static const std::array<float, 8> m_dbPerOctsOPZ;

	std::array<float, 64> timeInSecondsLut;
	std::array<float, 64> attcckTimeInSecondsLut;

	bool m_ksEn = false;
	Opzx7AdddrKeyScaleMode m_ksMode = Opzx7AdddrKeyScaleMode::MA7;
	bool m_ksrMA7 = false;
	int m_kslMA7 = 0;
	int m_ksrOPZ = 0;
	int m_kslOPZ = 0;
	int m_ksBp = 60;
	int m_ksLc = 0;
	int m_ksRc = 0;
	float m_ksLd = 0.0f;
	float m_ksRd = 0.0f;
	int m_ksRs = 0;

	bool m_sus = false;
	bool m_xof = false;
	bool m_kor = false;
	bool m_bypass = false;

	double m_sampleRate = 44100.0; // DAW Host Sample Rate

	float m_totalLevel = 0.0f;

	float m_attackInc = 0.0f;
	float m_decayDec = 0.0f;
	float m_sustainRateDec = 0.0f;
	float m_susReleaseDec = 0.0f;
	float m_releaseDec = 0.0f;
	float m_currentReleaseDec = 0.0f;
	float m_attackStartLevel = 0.0f; // アタック開始時のレベル
	float m_releaseTimeInc = 0.0f;

	// rrが無限大のとき、ストッパーの役目を果たす
	int m_releaseCounter = 0;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
	inline float getReleaseDec() const
	{
		if (m_sus) {
			return m_susReleaseDec;
		}
		else {
			return m_releaseDec;
		}
	}

	int m_noteNumber = 60; // C3

	// カーブモード用の変数
	int m_positionIndex = 1; // 1,2,3,4
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)

	int calcRateScaling() const;
	float calcLevelScalingDb() const;
public:
	Opzx7Adddr();
	void prepare(int posIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isBypass() const { return m_bypass; }
	bool isPlaying() const { return m_state != State::Idle; }
	bool isIdle() const { return m_state == State::Idle; }
	bool isRelease() const { return m_state == State::Release; }
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	void setParameters(const Opzx7AdddrParams& params);
	float noteOn(float velocity);
	void noteOff();
	void updateIncrements(int noteNumber);
	float updateEnvelopeState(float currentLevel);
	void setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl);
};
