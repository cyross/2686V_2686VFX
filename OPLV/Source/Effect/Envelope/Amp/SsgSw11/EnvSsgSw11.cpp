#include <algorithm>

#include "./EnvSsgSw11.h"

SsgSwEnv11::SsgSwEnv11() {
}

void SsgSwEnv11::prepare(int targetIndex, double sampleRate) {
    this->targetIndex = targetIndex;

    updateTargetSampleRate(sampleRate);
}

void SsgSwEnv11::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

void SsgSwEnv11::setParameters(const SsgSwEnv11Params& params) {
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
	this->r[7] = params.r7;
	this->l[7] = params.l7;
	this->r[8] = params.r8;
	this->l[8] = params.l8;
	this->r[9] = params.r9;
	this->l[9] = params.l9;
	this->r[10] = params.r10;
	this->l[10] = params.l10;
	this->r[11] = params.r11;
	this->l[11] = params.l11;

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

inline bool SsgSwEnv11::isReached(float inc, float current, float target) const {
    if (inc >= 0.0f && current >= target) return true;
    if (inc < 0.0f && current <= target) return true;

    return false;
}

void SsgSwEnv11::updateIncrements()
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
	rInc[6] = calcInc(this->r[6], this->l[6], this->l[5]);
	rInc[7] = calcInc(this->r[7], this->l[7], this->l[6]);
	rInc[8] = calcInc(this->r[8], this->l[8], this->l[7]);
	rInc[9] = calcInc(this->r[9], this->l[9], this->l[8]);
	rInc[10] = calcInc(this->r[10], this->l[10], this->l[9]);
    // rInc[11]はnoteOff時に動的計算するためここでは計算しない

    if (this->loop) {
        rIncLoop[this->steps] = calcInc(this->r[this->steps], this->l[this->loopTo], this->l[this->steps]);
    }
}

void SsgSwEnv11::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

void SsgSwEnv11::noteOn() {
    if (this->m_curveCore == nullptr) {
        this->state = State::S1;
        this->loopCounter = 0;
        this->currentLevel = this->l[0]; // Start Level から開始
    }
    else {
        this->state = State::S1;
        this->m_phaseProgress = 0.0f;
        this->loopCounter = 0;
        this->currentLevel = this->l[0]; // Start Level から開始
    }
}

void SsgSwEnv11::noteOff() {
    if (this->m_curveCore == nullptr) {
        this->state = State::S11;
        float r = std::max(0.001f, this->r[11]);
        // 現在のレベルからV11に向けて減衰・上昇する傾きを計算
        this->rInc[11] = (this->l[11] - this->currentLevel) / (r * (float)this->sampleRate);
    }
    else {
        this->state = State::S11;
        this->m_phaseProgress = 0.0f;
    }
}

void SsgSwEnv11::bypassedReleasedProcess() {
    this->state = State::Idle;
}

float SsgSwEnv11::process() {
    if (this->bypass) return 1.0f; // バイパス時は音量1.0(影響なし)を返す
    if (this->state == State::Idle) return this->currentLevel;

    if (this->m_curveCore == nullptr) {
        auto countUpLoopCounter = [&]() {
            if (loopCount > 0) {
                loopCounter++;
            }
            };

        bool isLoopTo = false;

        switch (this->state) {
        case State::Idle:
            return currentLevel;
        case State::S1:
            currentLevel += rInc[1];
            if (this->r[1] <= 0.001f || isReached(rInc[1], currentLevel, this->l[1])) {
                currentLevel = this->l[1];
                if (this->steps > 1)
                {
                    state = State::S2;
                }
            }

            return currentLevel;
        case State::S2:
            isLoopTo = this->loop && (int)this->state == this->steps;
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

            return currentLevel;
        case State::S3:
            isLoopTo = this->loop && (int)this->state == this->steps;
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

            return currentLevel;
        case State::S4:
            isLoopTo = this->loop && (int)this->state == this->steps;
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

            return currentLevel;
        case State::S5:
            isLoopTo = this->loop && (int)this->state == this->steps;
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

            return currentLevel;
        case State::S6:
            isLoopTo = this->loop && (int)this->state == this->steps;
            // ループ設定が有効で、かつ現在ループの終端になっている場合
            if (isLoopTo) {
                // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
                if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                    // ループ回数に達している場合は、通常のR2/L2の処理を行う
                    currentLevel += rInc[6];

                    if (this->r[6] <= 0.001f || isReached(rInc[6], currentLevel, this->l[6])) {
                        currentLevel = this->l[6];
                    }
                }
                else {
                    // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                    currentLevel += rIncLoop[6];

                    if (this->r[6] <= 0.001f || isReached(rIncLoop[6], currentLevel, this->l[this->loopTo])) {
                        currentLevel = this->l[this->loopTo];
                        state = (State)(this->loopTo + 1);

                        countUpLoopCounter();
                    }
                }
            }
            else {
                currentLevel += rInc[6];
                if (this->r[6] <= 0.001f || isReached(rInc[6], currentLevel, this->l[6])) {
                    currentLevel = this->l[6]; // S6到達後はホールド(サステイン)
                }
            }

            return currentLevel;
        case State::S7:
            isLoopTo = this->loop && (int)this->state == this->steps;
            // ループ設定が有効で、かつ現在ループの終端になっている場合
            if (isLoopTo) {
                // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
                if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                    // ループ回数に達している場合は、通常のR2/L2の処理を行う
                    currentLevel += rInc[7];

                    if (this->r[7] <= 0.001f || isReached(rInc[7], currentLevel, this->l[7])) {
                        currentLevel = this->l[7];
                    }
                }
                else {
                    // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                    currentLevel += rIncLoop[7];

                    if (this->r[7] <= 0.001f || isReached(rIncLoop[7], currentLevel, this->l[this->loopTo])) {
                        currentLevel = this->l[this->loopTo];
                        state = (State)(this->loopTo + 1);

                        countUpLoopCounter();
                    }
                }
            }
            else {
                currentLevel += rInc[7];
                if (this->r[7] <= 0.001f || isReached(rInc[7], currentLevel, this->l[7])) {
                    currentLevel = this->l[7]; // S7到達後はホールド(サステイン)
                }
            }

            return currentLevel;
        case State::S8:
            isLoopTo = this->loop && (int)this->state == this->steps;
            // ループ設定が有効で、かつ現在ループの終端になっている場合
            if (isLoopTo) {
                // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
                if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                    // ループ回数に達している場合は、通常のR2/L2の処理を行う
                    currentLevel += rInc[8];

                    if (this->r[8] <= 0.001f || isReached(rInc[8], currentLevel, this->l[8])) {
                        currentLevel = this->l[8];
                    }
                }
                else {
                    // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                    currentLevel += rIncLoop[8];

                    if (this->r[8] <= 0.001f || isReached(rIncLoop[8], currentLevel, this->l[this->loopTo])) {
                        currentLevel = this->l[this->loopTo];
                        state = (State)(this->loopTo + 1);

                        countUpLoopCounter();
                    }
                }
            }
            else {
                currentLevel += rInc[8];
                if (this->r[8] <= 0.001f || isReached(rInc[8], currentLevel, this->l[8])) {
                    currentLevel = this->l[8]; // S8到達後はホールド(サステイン)
                }
            }

            return currentLevel;
        case State::S9:
            isLoopTo = this->loop && (int)this->state == this->steps;
            // ループ設定が有効で、かつ現在ループの終端になっている場合
            if (isLoopTo) {
                // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
                if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                    // ループ回数に達している場合は、通常のR2/L2の処理を行う
                    currentLevel += rInc[9];

                    if (this->r[9] <= 0.001f || isReached(rInc[9], currentLevel, this->l[9])) {
                        currentLevel = this->l[9];
                    }
                }
                else {
                    // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                    currentLevel += rIncLoop[9];

                    if (this->r[9] <= 0.001f || isReached(rIncLoop[9], currentLevel, this->l[this->loopTo])) {
                        currentLevel = this->l[this->loopTo];
                        state = (State)(this->loopTo + 1);

                        countUpLoopCounter();
                    }
                }
            }
            else {
                currentLevel += rInc[9];
                if (this->r[9] <= 0.001f || isReached(rInc[9], currentLevel, this->l[9])) {
                    currentLevel = this->l[9]; // S9到達後はホールド(サステイン)
                }
            }

            return currentLevel;
        case State::S10:
            isLoopTo = this->loop && (int)this->state == this->steps;
            // ループ設定が有効で、かつ現在ループの終端になっている場合
            if (isLoopTo) {
                // ループ回数が設定されている場合は、ループ回数に達しているかをチェック
                if (this->loopCount > 0 && this->loopCounter == this->loopCount) {
                    // ループ回数に達している場合は、通常のR2/L2の処理を行う
                    currentLevel += rInc[10];

                    if (this->r[10] <= 0.001f || isReached(rInc[10], currentLevel, this->l[10])) {
                        currentLevel = this->l[10];
                    }
                }
                else {
                    // ループ回数に達していない場合は、ループ先のレベルに向かって変化させる
                    currentLevel += rIncLoop[10];

                    if (this->r[10] <= 0.001f || isReached(rIncLoop[10], currentLevel, this->l[this->loopTo])) {
                        currentLevel = this->l[this->loopTo];
                        state = (State)(this->loopTo + 1);

                        countUpLoopCounter();
                    }
                }
            }
            else {
                currentLevel += rInc[10];
                if (this->r[10] <= 0.001f || isReached(rInc[10], currentLevel, this->l[10])) {
                    currentLevel = this->l[10]; // S10到達後はホールド(サステイン)
                }
            }

            return currentLevel;
        case State::S11:
            currentLevel += rInc[11];
            if (this->r[11] <= 0.001f || isReached(rInc[11], currentLevel, this->l[11])) {
                currentLevel = this->l[11];
                state = State::Idle;
            }

            return currentLevel;
        }
    }
    else {
        int s = (int)this->state; // S1=1, S2=2 ... S6=11

        // =========================================================
        // リリースフェーズ (S11)
        // =========================================================
        if (s == 11) {
            if (this->m_phaseProgress == 0.0f) {
                this->m_releaseStartLevel = this->currentLevel;
            }

            float targetLevel = this->l[11];
            float rateVal = std::max(0.001f, this->r[11]);
            float deltaX = 1.0f / (rateVal * (float)this->sampleRate);

            this->m_phaseProgress += deltaX;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 1.0f;
                this->currentLevel = targetLevel;
                this->state = State::Idle;
                return this->currentLevel;
            }

            float y = m_curveCore->processAudio(
                this->targetIndex,
                (int)CurveParams::Target::SsgSwEnv11,
                (int)CurveParams::TargetSsgSwEnv11::R11,
                this->m_phaseProgress
            );

            this->currentLevel = this->m_releaseStartLevel + y * (targetLevel - this->m_releaseStartLevel);

            return this->currentLevel;
        }

        // =========================================================
        // S1 〜 S10 フェーズ (抽象化ロジック)
        // =========================================================
        bool isLoopTo = this->loop && (s == this->steps);
        bool doLoop = isLoopTo && (this->loopCount == 0 || this->loopCounter < this->loopCount);

        // 1. 始点と終点の決定
        float startLevel = this->l[s - 1];
        float targetLevel = doLoop ? this->l[this->loopTo] : this->l[s];
        float rateVal = this->r[s];

        // 2. 時間(x)を進める
        float deltaX = rateVal <= 0.001f ? 1.0f : 1.0f / (rateVal * (float)this->sampleRate);
        this->m_phaseProgress += deltaX;

        bool phaseEnded = this->m_phaseProgress >= 1.0f;
        if (phaseEnded) {
            this->m_phaseProgress = 1.0f;
        }

        // 3. カーブ取得 (y)
        // (TargetSsgSwEnv::R1 = 0, R2 = 1 ... LoopTo = 6)
        float y = m_curveCore->processAudio(
            this->targetIndex,
            (int)CurveParams::Target::SsgSwEnv11,
            doLoop ? (int)CurveParams::TargetSsgSwEnv11::LoopTo : (s - 1),
            this->m_phaseProgress
        );
        this->currentLevel = startLevel + y * (targetLevel - startLevel);

        // 4. フェーズ終了時の状態遷移
        if (phaseEnded) {
            this->currentLevel = targetLevel; // ズレ防止のためターゲット値にピッタリ合わせる

            if (doLoop) {
                // ループバック
                this->m_phaseProgress = 0.0f;
                this->state = (State)(this->loopTo + 1);
                if (this->loopCount > 0) this->loopCounter++;
            }
            else {
                // 次のフェーズへ進むか、Hold(サステイン)する
                if (this->steps > s) {
                    this->m_phaseProgress = 0.0f;
                    this->state = (State)(s + 1);
                }
                else {
                    // ループしない最終ステップに到達した場合は現在状態を維持 (Hold)
                    // m_phaseProgress は 1.0 のまま固定され、音量が維持されます。
                }
            }
        }

        return this->currentLevel;
    }

    return this->currentLevel;
}
