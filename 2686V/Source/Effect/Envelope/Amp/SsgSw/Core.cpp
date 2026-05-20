#include <algorithm>

#include "./Core.h"

void SsgSwEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void SsgSwEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
    this->updateIncrements();
}

void SsgSwEnv::setParameters(const SsgSwEnvParams& params) {
	this->bypass = params.bypass;
	this->steps = params.steps;
    this->stl = params.stl;
    this->r1 = params.r1;
	this->l1 = params.l1;
	this->r2 = params.r2;
	this->l2 = params.l2;
	this->r3 = params.r3;
	this->l3 = params.l3;
	this->r4 = params.r4;
	this->l4 = params.l4;
	this->r5 = params.r5;
	this->l5 = params.l5;
	this->r6 = params.r6;
	this->l6 = params.l6;
    this->updateIncrements();
}

void SsgSwEnv::noteOn() {
    state = State::S1;
    currentLevel = this->stl; // Start Level から開始
}

void SsgSwEnv::noteOff() {
    state = State::S6;
    float r = std::max(0.001f, this->r6);
    // 現在のレベルからV6に向けて減衰・上昇する傾きを計算
    r6Inc = (this->l6 - currentLevel) / (r * (float)sampleRate);
}

void SsgSwEnv::bypassedReleasedProcess() {
    state = State::Idle;
}

inline bool SsgSwEnv::isReached(float inc, float current, float target) const {
    if (inc >= 0.0f && current >= target) return true;
    if (inc < 0.0f && current <= target) return true;

    return false;
}

float SsgSwEnv::process() {
    if (this->bypass) return 1.0f; // バイパス時は音量1.0(影響なし)を返す

    switch (state) {
    case State::S1:
        currentLevel += r1Inc;
        if (this->r1 <= 0.001f || isReached(r1Inc, currentLevel, this->l1)) {
            currentLevel = this->l1;
            if (this->steps > 0)
            {
                state = State::S2;
            }
        }
        break;
    case State::S2:
        currentLevel += r2Inc;
        if (this->r2 <= 0.001f || isReached(r2Inc, currentLevel, this->l2)) {
            currentLevel = this->l2;
            if (this->steps > 1)
            {
                state = State::S3;
            }
        }
        break;
    case State::S3:
        currentLevel += r3Inc;
        if (this->r3 <= 0.001f || isReached(r3Inc, currentLevel, this->l3)) {
            currentLevel = this->l3;
            if (this->steps > 2)
            {
                state = State::S4;
            }
        }
        break;
    case State::S4:
        currentLevel += r4Inc;
        if (this->r4 <= 0.001f || isReached(r4Inc, currentLevel, this->l4)) {
            currentLevel = this->l4;
            if (this->steps > 3)
            {
                state = State::S5;
            }
        }
        break;
    case State::S5:
        currentLevel += r5Inc;
        if (this->r5 <= 0.001f || isReached(r5Inc, currentLevel, this->l5)) {
            currentLevel = this->l5; // S5到達後はホールド(サステイン)
        }
        break;
    case State::S6:
        currentLevel += r6Inc;
        if (this->r6 <= 0.001f || isReached(r6Inc, currentLevel, this->l6)) {
            currentLevel = this->l6; state = State::Idle;
        }
        break;
    default:
        break;
    }

    return currentLevel;
}

void SsgSwEnv::updateIncrements()
{
    if (sampleRate <= 0.0) return;

    auto calcInc = [&](float r, float target, float start) {
        // Rが0に近い場合は一瞬で到達させるためのフラグ代わり
        return r <= 0.001f ? (target - start) : (target - start) / (r * (float)sampleRate);
        };

    r1Inc = calcInc(this->r1, this->l1, this->stl);
    r2Inc = calcInc(this->r2, this->l2, this->l1);
    r3Inc = calcInc(this->r3, this->l3, this->l2);
    r4Inc = calcInc(this->r4, this->l4, this->l3);
    r5Inc = calcInc(this->r5, this->l5, this->l4);
    // r6IncはnoteOff時に動的計算するためここでは計算しない
}

void SsgSwEnv::updateSampleRate(double newSampleRate) {
	this->sampleRate = newSampleRate;
    this->updateIncrements();
}
