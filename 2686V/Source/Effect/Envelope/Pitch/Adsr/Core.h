#pragma once

#include "./Params.h"

class PitchAdsrEnv {
	enum class State { Idle, Attack, Decay, Sustain, Release };
	State state = State::Idle;

	float ar = 0.0f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.0f;
	int stl = 0;
	int atl = 0;
	int ssl = 0;
	int rll = 0;
	bool bypass = false;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	float phaseProgress = 0.0f;     // 現在のフェーズの進行度 (0.0f 〜 1.0f)
	float currentCents = 0.0f;      // 現在出力されているピッチ補正値 (セント)
	float releaseStartCents = 0.0f; // ノートオフされた瞬間のピッチ補正値
	float attackDelta = 0.0f;
	float decayDelta = 0.0f;
	float releaseDelta = 0.0f;

	void updateIncrements();
public:
	void prepare(double sampleRate);
	void setParameters(const PitchAdsrParams& params);
	void noteOn();
	void noteOff();
	float process(float phaseDelta);
	bool isPlaying() const { return state != State::Idle; }
	bool isIdle() const { return state == State::Idle; }
	bool isRelease() const { return state == State::Release; }
	bool isBypassed() const { return bypass; }
	void updateSampleRate(double newSampleRate);
};
