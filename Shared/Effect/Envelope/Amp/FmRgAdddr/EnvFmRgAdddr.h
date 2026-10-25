#pragma once

#include <functional>

#include "./EnvFmRgAdddrParams.h"
#include "Shared/Core/Synth/LivePosition.h"
#include "../../../KeyScale/Opn/KSOpn.h"
#include "../../../KeyScale/Opp/KSOpp.h"
#include "../../../../Advanced/Curve/AdvancedCurve.h"

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

	// 押したときに TL で下げた量 (dB)
	float m_tlDb = 0.0f;

	float attackInc = 0.0f;
	float decayDec = 0.0f;
	float releaseDec = 0.0f;
	float sustainRateDec = 0.0f;
	float m_attackStartLevel = 0.0f; // アタック開始時のレベル
	float releaseTimeInc = 0.0f;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	int m_noteNumber = 60; // C3

	// カーブモード用の変数
	int positionIndex = 1; // 1,2,3,4
	CurveCore* m_curveCore = nullptr;

	// カーブモード用の時間管理変数
	float m_phaseProgress = 0.0f; // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float m_releaseStartLevel = 0.0f; // リリース開始時のレベル(Releaseの始点Y)

	int calcRateScaling() const;
	float calcLevelScalingDb() const;
	// 鳴っている位置 (グラフの縦線。4.3.0)
	LivePosition* m_livePos = nullptr;
	LiveLevelWriter m_live;

	float advanceEnvelope(float currentLevel);

	int livePhase() const
	{
		switch (state) {
		case State::Attack: return LivePosition::attack;
		case State::Decay: return LivePosition::decay;
		case State::Sustain: return LivePosition::sustain;
		case State::Release: return LivePosition::release;
		default: return LivePosition::idle;
		}
	}

	// 区間の行き先の音量。サスティンは SR (D2R) で下がるなら 0 へ
	float liveTarget() const
	{
		switch (state) {
		case State::Attack: return 1.0f;
		case State::Decay: return m_sustain;
		default: return 0.0f;
		}
	}
public:

	// 押したときに TL で下げた量 (dB)。KSL は含まない。TL ENV がこれを
	// 段の TL に置き換えるのに使う。バイパスのあいだは 0。
	float tlDb() const { return m_tlDb; }
	FmRgAdddr();
	void prepare(int posIndex, double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	bool isBypass() const { return bypass; }
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	void setCurveCore(CurveCore* core) { m_curveCore = core; }
	void setParameters(const FmRgAdddrParams& params);
	float noteOn(float velocity, int noteNumber);
	void noteOff();
	void updateIncrementsWithKeyScale(int noteNumber);
	// 1 サンプル進める。鳴っている位置もここで書く (グラフの縦線。4.3.0)
	float updateEnvelopeState(float currentLevel)
	{
		const float level = advanceEnvelope(currentLevel);

		if (m_live.active()) m_live.write(livePhase(), level, liveTarget());

		return level;
	}
	void setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl);
	void bypassedReleasedProcess();
};
