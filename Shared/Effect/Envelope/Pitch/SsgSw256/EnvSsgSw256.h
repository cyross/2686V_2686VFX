#pragma once

#include "./EnvSsgSw256Params.h"
#include "../../Step/EnvStep.h"

// SSG SW PITCH ENV[256]。段の値 (セント) を周波数比にして phaseDelta へ掛ける。
// 中身は 256 段のエンベロープ (StepEnv)。
//
// リリースを走り終えたあとは素通しする (Use Endl のときだけ ENDL を保つ)。
// 3.6.3 までの SSG SW PITCH ENV[11] と同じ。
class SsgSwPEnv256 {
	StepEnv env;
public:
	void prepare(int targetIndex, double sampleRate);
	void updateSampleRate(double newSampleRate) { env.updateTargetSampleRate(newSampleRate); }
	void updateTargetSampleRate(double newSampleRate) { env.updateTargetSampleRate(newSampleRate); }
	bool isPlaying() const { return env.isPlaying(); }
	bool isIdle() const { return env.isIdle(); }
	bool isRelease() const { return env.isRelease(); }
	bool isBypass() const { return env.isBypass(); }
	void setParameters(const SsgSwPEnv256Params& params) { env.setParameters(params); }
	void setCurveCore(CurveCore* core) { env.setCurveCore(core); }
	void noteOn() { env.noteOn(); }
	void noteOff() { env.noteOff(); }
	float process(float phaseDelta);
	float bypassedReleasedProcess() { env.stop(); return 0.0f; }
	float bypassedProcess() { return 1.0f; }
};
