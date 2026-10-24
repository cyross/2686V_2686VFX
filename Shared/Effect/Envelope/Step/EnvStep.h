#pragma once

#include <algorithm>
#include <array>

#include "Shared/Core/Synth/StepTable.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"

// ============================================================================
// 256 段のエンベロープの本体
// ============================================================================
// SSG SW AMP ENV[256] / SSG SW PITCH ENV[256] (と、同じ形の TL ENV など) が
// 共通に使う。値の意味 (音量・セント・TL など) は持たず、STL から段ごとに
// L1, L2, ... へ R 秒かけて進む数だけを返す。
//
//   押す        : STL から始め、R1 秒で L1、R2 秒で L2 … と進む
//   STEPS 段目  : LOOP が入っていれば L.TO の段へ戻る (CNT 回。0 は無限)。
//                 入っていなければ、離すまで L(STEPS) を保つ (サステイン。KEEP でも同じ)
//   Rich Mode   : LOOP MODE が Rich なら、L.TO / CNT の代わりに表のループの組
//                 (START・END・COUNT) で戻る (StepLoop.h)。END の段を終えたら
//                 START の段へ、L(END) から L(START) へ R(START) 秒かけて進む
//   離す        : そのときの値から RR 秒で LR へ。走り終えたら LR を保つ
//                 (Use Endl のときは ENDL)
//   KEEP        : 段のあいだ値を斜めに繋がず、段の頭の値を保つ
//
// 段の R / L は StepTable (プロセッサが持つ表) から写す。表の版が変わった
// ときだけ写し直す。
struct StepEnvParams
{
	bool bypass = true;
	int steps = 10;
	bool loop = false;
	int loopTo = 0;
	int loopCount = 0;

	// LOOP MODE (StepLoop::Mode)。Rich のときは表のループの組を使う
	int loopMode = StepLoop::Original;

	float stl = 0.0f;   // 押した瞬間の値
	float lr = 0.0f;    // 離したあとに向かう値
	float rr = 0.0f;    // 離してから LR へ届くまで (秒)

	// リリースを走り終えたあとに保つ値。endlEnable のときだけ使う。
	float endl = 0.0f;
	bool endlEnable = false;

	bool keep = false;

	// 段の R / L。nullptr なら表を持たない (段の値はすべて 0)。
	const StepTable* table = nullptr;

	// PARAM ENV (4.3.0)。0 以上なら、STL・段の L・LR・ENDL を表の値の組 (指し先ごと) から取る
	int levelSet = -1;
};

class StepEnv
{
public:
	static constexpr int maxSteps = StepTable::size;

	// curveTarget は CurveParams::Target のどれか (カーブを持たないなら -1)。
	// 段 s のカーブは R(s) を使う。R1〜R10 があるので、11 段目からは R10 を使う。
	// リリースは R11、ループで戻る段は LoopTo。
	void prepare(int targetIndex, int curveTarget, double sampleRate);
	void updateTargetSampleRate(double newSampleRate) { sampleRate = newSampleRate; }

	void setParameters(const StepEnvParams& params);
	void setCurveCore(CurveCore* core) { curveCore = core; }

	void noteOn();
	void noteOff();

	// 1 サンプル進めて値を返す。KEEP なら段の頭の値。
	float process();

	// 値だけを読む (進めない)
	float value() const { return keep ? keepLevel : current; }

	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	bool isBypass() const { return bypass; }

	// バイパスのまま離されたとき。走らせずに終える。
	void stop() { state = State::Idle; }

	bool usesEndl() const { return endlEnable; }
	float endLevel() const { return endl; }

private:
	enum class State { Idle, Running, Release };

	State state = State::Idle;

	bool bypass = true;
	int steps = 1;
	bool loop = false;
	int loopTo = 0;
	int loopCount = 0;
	bool rich = false;
	float lr = 0.0f;
	float rr = 0.0f;
	float endl = 0.0f;
	bool endlEnable = false;
	bool keep = false;

	// [0] が STL、[1] 以降が段の値。R は [1] から。
	std::array<float, maxSteps + 1> r{};
	std::array<float, maxSteps + 1> l{};

	const StepTable* table = nullptr;
	std::uint32_t tableVersion = 0;

	// 写している値の組 (PARAM ENV)。-1 は組を使わない
	int levelSet = -1;

	double sampleRate = 44100.0;

	// いまの段と、その段の行き先
	int step = 1;
	bool holding = false;
	bool looping = false;
	int loopCounter = 0;

	// Rich Mode のループの進み具合 (ループの組は表から直に引く)
	StepLoop::State richState;
	float current = 0.0f;
	float startLevel = 0.0f;
	float target = 0.0f;
	float inc = 0.0f;
	bool instant = false;
	float progress = 0.0f;

	// KEEP で返す値。段が変わるたびに取り直す。
	float keepLevel = 0.0f;

	// カーブ
	int targetIndex = 0;
	int curveTarget = -1;
	CurveCore* curveCore = nullptr;

	void beginStep(int s);
	void advance();
	void hold();
	float levelOf(int index) const { return l[(size_t)std::clamp(index, 0, maxSteps)]; }
	float curve(int param, float x) const;
	bool useCurve() const { return curveCore != nullptr && curveTarget >= 0; }
};
