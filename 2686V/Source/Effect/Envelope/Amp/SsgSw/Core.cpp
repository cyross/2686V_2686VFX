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
	this->loop = params.loop;
	this->loopTo = params.loopTo;
	this->loopCount = params.loopCount;
    this->l[0] = params.stl;
    this->r[1] = params.r1;
	this->l[1] = params.l1;
	this->r[2] = params.r2;
	this->l[2] = params.l2;
	this->r[3] = params.r3;
	this->l[3] = params.l3;
	this->r[4] = params.r4;
	this->l[4] = params.l4;
	this->r[5] = params.r5;
	this->l[5] = params.l5;
	this->r[6] = params.r6;
	this->l[6] = params.l6;

    // GUIでも loop=true 時のガードはやっているが、念の為ロジックでもチェックする
    if (this->loop) {
        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (this->steps < 2) {
            this->steps = 2;
        }

        // Steps - LoopTo が 2未満のときは、LoopTo を Steps - 2 にする
        if (this->steps - this->loopTo < 2) {
            this->loopTo = this->steps - 2;
        }
    }

    this->updateIncrements();
}

void SsgSwEnv::noteOn() {
    this->state = State::S1;
    this->loopCounter = 0;
    this->currentLevel = this->l[0]; // Start Level から開始
}

void SsgSwEnv::noteOff() {
    this->state = State::S6;
    float r = std::max(0.001f, this->r[6]);
    // 現在のレベルからV6に向けて減衰・上昇する傾きを計算
    this->rInc[6] = (this->l[6] - this->currentLevel) / (r * (float)this->sampleRate);
}

void SsgSwEnv::bypassedReleasedProcess() {
    this->state = State::Idle;
}

inline bool SsgSwEnv::isReached(float inc, float current, float target) const {
    if (inc >= 0.0f && current >= target) return true;
    if (inc < 0.0f && current <= target) return true;

    return false;
}

float SsgSwEnv::process() {
    if (this->bypass) return 1.0f; // バイパス時は音量1.0(影響なし)を返す

	auto countUpLoopCounter = [&]() {
		if (loopCount > 0) {
			loopCounter++;
		}
		};

	bool isLoopTo = this->loop && (int)this->state == this->steps;

    switch (state) {
    case State::S1:
        currentLevel += rInc[1];
        if (this->r[1] <= 0.001f || isReached(rInc[1], currentLevel, this->l[1])) {
            currentLevel = this->l[1];
            if (this->steps > 1)
            {
                state = State::S2;
            }
        }
        break;
    case State::S2:
		// ループ設定が有効で、かつ現在ループの終端になっている場合
        if (isLoopTo) {
			// ループ回数が設定されている場合は、ループ回数に達しているかをチェック
            if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
				// ループ回数に達している場合は、通常のR2/L2の処理を行う
                currentLevel += rInc[2];

                if (this->r[2] <= 0.001f || isReached(rInc[2], currentLevel, this->l[2])) {
                    currentLevel = this->l[2];
                }
            }
            else {
				// ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                currentLevel += rIncLoop[2];

                if (this->r[2] <= 0.001f || isReached(rIncLoop[2], currentLevel, this->l[this->loopTo])) {
                    currentLevel = this->l[this->loopTo];
                    state = (State)(this->loopTo + 1);

                    countUpLoopCounter();
                }
            }
        }
        else {
            currentLevel += rInc[2];

            if (this->r[2] <= 0.001f || isReached(rInc[2], currentLevel, this->l[2])) {
                currentLevel = this->l[2];
                if (this->steps > 2)
                {
                    state = State::S3;
                }
            }
        }
        break;
    case State::S3:
        // ループ設定が有効で、かつ現在ループの終端になっている場合
        if (isLoopTo) {
            // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
            if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                // ループ回数に達している場合は、通常のR2/L2の処理を行う
                currentLevel += rInc[3];

                if (this->r[3] <= 0.001f || isReached(rInc[3], currentLevel, this->l[3])) {
                    currentLevel = this->l[3];
                }
            }
            else {
                // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                currentLevel += rIncLoop[3];

                if (this->r[3] <= 0.001f || isReached(rIncLoop[3], currentLevel, this->l[this->loopTo])) {
                    currentLevel = this->l[this->loopTo];
                    state = (State)(this->loopTo + 1);

                    countUpLoopCounter();
                }
            }
        }
        else {
            currentLevel += rInc[3];
            if (this->r[3] <= 0.001f || isReached(rInc[3], currentLevel, this->l[3])) {
                currentLevel = this->l[3];
                if (this->steps > 3)
                {
                    state = State::S4;
                }
            }
        }
        break;
    case State::S4:
        // ループ設定が有効で、かつ現在ループの終端になっている場合
        if (isLoopTo) {
            // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
            if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                // ループ回数に達している場合は、通常のR2/L2の処理を行う
                currentLevel += rInc[4];

                if (this->r[4] <= 0.001f || isReached(rInc[4], currentLevel, this->l[4])) {
                    currentLevel = this->l[4];
                }
            }
            else {
                // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                currentLevel += rIncLoop[4];

                if (this->r[4] <= 0.001f || isReached(rIncLoop[4], currentLevel, this->l[this->loopTo])) {
                    currentLevel = this->l[this->loopTo];
                    state = (State)(this->loopTo + 1);

                    countUpLoopCounter();
                }
            }
        }
        else {
            currentLevel += rInc[4];
            if (this->r[4] <= 0.001f || isReached(rInc[4], currentLevel, this->l[4])) {
                currentLevel = this->l[4];
                if (this->steps > 4)
                {
                    state = State::S5;
                }
            }
        }
        break;
    case State::S5:
        // ループ設定が有効で、かつ現在ループの終端になっている場合
        if (isLoopTo) {
            // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
            if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                // ループ回数に達している場合は、通常のR2/L2の処理を行う
                currentLevel += rInc[5];

                if (this->r[5] <= 0.001f || isReached(rInc[5], currentLevel, this->l[5])) {
                    currentLevel = this->l[5];
                }
            }
            else {
                // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                currentLevel += rIncLoop[5];

                if (this->r[5] <= 0.001f || isReached(rIncLoop[5], currentLevel, this->l[this->loopTo])) {
                    currentLevel = this->l[this->loopTo];
                    state = (State)(this->loopTo + 1);

                    countUpLoopCounter();
                }
            }
        }
        else {
            currentLevel += rInc[5];
            if (this->r[5] <= 0.001f || isReached(rInc[5], currentLevel, this->l[5])) {
                currentLevel = this->l[5]; // S5到達後はホールド(サステイン)
            }
        }
        break;
    case State::S6:
        currentLevel += rInc[6];
        if (this->r[6] <= 0.001f || isReached(rInc[6], currentLevel, this->l[6])) {
            currentLevel = this->l[6];
            state = State::Idle;
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

    rInc[1] = calcInc(this->r[1], this->l[1], this->l[0]);
    rInc[2] = calcInc(this->r[2], this->l[2], this->l[1]);
    rInc[3] = calcInc(this->r[3], this->l[3], this->l[2]);
    rInc[4] = calcInc(this->r[4], this->l[4], this->l[3]);
    rInc[5] = calcInc(this->r[5], this->l[5], this->l[4]);
    // rInc[6]はnoteOff時に動的計算するためここでは計算しない

    if (this->loop) {
        rIncLoop[this->steps] = calcInc(this->r[this->steps], this->l[this->loopTo], this->l[this->steps]);
    }
}

void SsgSwEnv::updateSampleRate(double newSampleRate) {
	this->sampleRate = newSampleRate;
    this->updateIncrements();
}
