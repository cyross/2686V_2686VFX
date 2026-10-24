#include "./EnvStep.h"

#include <algorithm>

namespace
{
	// これより短い R は、待たずにその場で行き先へ移る
	constexpr float instantRate = 0.001f;

	// カーブの並び (CurveParams::TargetSsgSwEnv256 と同じ)。R1〜R10 が段、R11 が
	// リリース、その次がループで戻る段。
	constexpr int curveStepParams = 10;
	constexpr int curveRelease = 10;
	constexpr int curveLoopTo = 11;
}

void StepEnv::prepare(int index, int target, double rate)
{
	targetIndex = index;
	curveTarget = target;
	sampleRate = rate;
}

void StepEnv::setParameters(const StepEnvParams& p)
{
	bypass = p.bypass;
	steps = std::clamp(p.steps, 1, maxSteps);
	loop = p.loop;
	loopTo = p.loopTo;
	loopCount = p.loopCount;
	rich = p.loopMode == StepLoop::Rich;
	lr = p.lr;
	rr = p.rr;
	endl = p.endl;
	endlEnable = p.endlEnable;
	keep = p.keep;
	l[0] = p.stl;

	// 画面でも直しているが、念のためここでもループできる形にする。
	// Rich Mode は 1 段でもループできるので直さない。
	if (loop && !rich) {
		if (steps < 2) steps = 2;
		if (steps - loopTo < 2) loopTo = steps - 2;
	}

	loopTo = std::clamp(loopTo, 0, steps - 1);

	if (p.table != table || p.levelSet != levelSet) {
		table = p.table;
		levelSet = p.levelSet;
		tableVersion = 0;
	}

	// 表は版が変わったときだけ写す
	if (table != nullptr) {
		const auto v = table->version.load(std::memory_order_acquire);

		if (v != tableVersion) {
			tableVersion = v;

			const bool fromSet = levelSet >= 0 && levelSet < table->setCount();

			for (int s = 1; s <= maxSteps; ++s) {
				r[(size_t)s] = table->rate(s);
				l[(size_t)s] = fromSet ? table->setValue(levelSet, s) : table->level(s);
			}
		}

		// PARAM ENV。STL・LR・ENDL も指し先の組から取る
		if (levelSet >= 0 && levelSet < table->setCount()) {
			l[0] = table->setValue(levelSet, StepTable::setStl);
			lr = table->setValue(levelSet, StepTable::setLr);
			endl = table->setValue(levelSet, StepTable::setEndl);
		}
	}
	else {
		std::fill(r.begin() + 1, r.end(), 0.0f);
		std::fill(l.begin() + 1, l.end(), 0.0f);
	}
}

float StepEnv::curve(int param, float x) const
{
	return curveCore->processAudio(targetIndex, curveTarget, param, x);
}

void StepEnv::noteOn()
{
	loopCounter = 0;
	richState.clear();
	current = l[0];
	keepLevel = current;
	state = State::Running;

	beginStep(1);
}

void StepEnv::noteOff()
{
	state = State::Release;
	holding = false;
	startLevel = current;
	target = lr;
	progress = 0.0f;
	instant = false;

	const float rate = std::max(instantRate, rr);

	inc = sampleRate > 0.0 ? (lr - current) / (rate * (float)sampleRate) : (lr - current);

	// KEEP の値はここでは取り直さない。[11] と同じく、リリースのあいだは
	// 前の段の値を保ち、走り終えたところで LR (ENDL) へ移る。
}

// 段 s を始める。行き先と傾きを決める。
void StepEnv::beginStep(int s)
{
	step = s;
	holding = false;
	looping = loop && !rich && s == steps && (loopCount == 0 || loopCounter < loopCount);
	startLevel = current;
	target = looping ? levelOf(loopTo) : levelOf(s);
	progress = 0.0f;

	const float rate = r[(size_t)s];

	instant = rate <= instantRate || sampleRate <= 0.0;
	inc = instant ? (target - current) : (target - current) / (rate * (float)sampleRate);
}

void StepEnv::advance()
{
	if (state == State::Idle) return;

	if (state == State::Release) {
		bool ended = false;

		if (useCurve()) {
			progress += 1.0f / (std::max(instantRate, rr) * (float)sampleRate);
			ended = progress >= 1.0f;

			if (!ended) current = startLevel + curve(curveRelease, progress) * (target - startLevel);
		}
		else {
			current += inc;
			ended = (inc >= 0.0f && current >= target) || (inc < 0.0f && current <= target);
		}

		if (ended) {
			current = target;
			state = State::Idle;

			if (endlEnable) current = endl;

			keepLevel = current;
		}

		return;
	}

	// 最後の段を走り終えて保っているところ (サステイン)。値を動かしたときに追うよう、
	// 毎回取り直す。KEEP のときも、ここからは最後の段の値を出す。
	if (holding) {
		current = levelOf(step);
		keepLevel = current;
		return;
	}

	bool ended = instant;

	if (!ended) {
		if (useCurve()) {
			progress += 1.0f / (r[(size_t)step] * (float)sampleRate);
			ended = progress >= 1.0f;

			const int param = looping ? curveLoopTo : std::min(step, curveStepParams) - 1;

			if (!ended) current = startLevel + curve(param, progress) * (target - startLevel);
		}
		else {
			current += inc;
			ended = (inc >= 0.0f && current >= target) || (inc < 0.0f && current <= target);
		}
	}

	if (!ended) return;

	current = target;

	if (looping) {
		if (loopCount > 0) ++loopCounter;

		beginStep(loopTo + 1);
	}
	else if (loop && rich) {
		// 表のループの組で、次に進む段を決める。STEPS を越えたら最後の段を保つ
		const int nextStep = table != nullptr ? StepLoop::next(table->loopView(), table->loopCount(), steps, richState, step) : step + 1;

		if (nextStep > steps) {
			hold();
			return;
		}

		beginStep(nextStep);
	}
	else if (step < steps) {
		beginStep(step + 1);
	}
	else {
		hold();
		return;
	}

	// 段が変わったので、KEEP で保つ値を取り直す
	keepLevel = current;
}

// 最後の段を走り終えた。離すまで最後の段の値を保つ (サステイン)。
// KEEP のときも最後の段の値へ移る。離すと、この値から RR 秒で LR へ向かう。
void StepEnv::hold()
{
	holding = true;
	keepLevel = current;
}

float StepEnv::process()
{
	advance();

	return keep ? keepLevel : current;
}
