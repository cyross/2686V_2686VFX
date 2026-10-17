#pragma once

#include "./EnvSsgSw256Params.h"
#include "../../Step/EnvStep.h"

// SSG SW AMP ENV[256]。音量へ掛ける倍率を返す。中身は 256 段の
// エンベロープ (StepEnv) で、バイパスのあいだは 1 (掛けない) を返す。
//
// 3.6.3 までの SSG SW AMP ENV[11] (10 段 + リリース) と SSG SW AMP ENV
// (6 段) の代わり。どちらも、段を表へ写せばこれで同じ動きになる。
class SsgSwEnv256 {
	StepEnv env;
public:
	void prepare(int targetIndex, double sampleRate);
	void updateSampleRate(double newSampleRate) { env.updateTargetSampleRate(newSampleRate); }
	void updateTargetSampleRate(double newSampleRate) { env.updateTargetSampleRate(newSampleRate); }
	bool isPlaying() const { return env.isPlaying(); }
	bool isIdle() const { return env.isIdle(); }
	bool isRelease() const { return env.isRelease(); }
	bool isBypass() const { return env.isBypass(); }
	void setParameters(const SsgSwEnv256Params& params) { env.setParameters(params); }
	void setCurveCore(CurveCore* core) { env.setCurveCore(core); }
	void noteOn() { env.noteOn(); }
	void noteOff() { env.noteOff(); }
	float process();
	void bypassedReleasedProcess() { env.stop(); }
};
