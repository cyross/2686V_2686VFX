#include <algorithm>

#include "./EnvSsgSw11.h"

SsgSwPEnv11::SsgSwPEnv11() {
}

void SsgSwPEnv11::prepare(int targetIndex, double sampleRate) {
    this->targetIndex = targetIndex;

    updateTargetSampleRate(sampleRate);
}

void SsgSwPEnv11::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
    this->updateIncrements();
}

void SsgSwPEnv11::setParameters(const SsgSwPEnv11Params& params) {
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

inline bool SsgSwPEnv11::isReached(float inc, float current, float target) const {
    if (inc >= 0.0f && current >= target) return true;
    if (inc < 0.0f && current <= target) return true;

    return false;
}

void SsgSwPEnv11::updateIncrements()
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

void SsgSwPEnv11::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

void SsgSwPEnv11::noteOn() {
    this->state = State::S1;
    this->m_phaseProgress = 0.0f;
    this->loopCounter = 0;
    this->currentLevel = this->l[0]; // Start Level から開始
}

void SsgSwPEnv11::noteOff() {
    this->state = State::S11;
    this->m_phaseProgress = 0.0f;
}

float SsgSwPEnv11::bypassedReleasedProcess() {
    this->state = State::Idle;
    return 0.0f;
}

float SsgSwPEnv11::bypassedProcess() {
    return 1.0f;
}

float SsgSwPEnv11::process(float phaseDelta) {
    if (this->bypass) return phaseDelta;
    if (this->state == State::Idle) return phaseDelta;

    if (this->m_curveCore == nullptr) {
        return this->currentLevel;
    }

    int s = (int)this->state; // S1=1, S2=2 ... S11=11

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
        }
        else {
            float y = m_curveCore->process(
                this->targetIndex,
                (int)CurveParams::Target::SsgSwPEnv11,
                (int)CurveParams::TargetSsgSwPEnv11::R11,
                this->m_phaseProgress
            );
            this->currentLevel = this->m_releaseStartLevel + y * (targetLevel - this->m_releaseStartLevel);
        }
    }
    else {
        // =========================================================
        // S1 〜 S10 フェーズ
        // =========================================================
        bool isLoopTo = this->loop && (s == this->steps);
        bool doLoop = isLoopTo && (this->loopCount == 0 || this->loopCounter < this->loopCount);

        float startLevel = this->l[s - 1];
        float targetLevel = doLoop ? this->l[this->loopTo] : this->l[s];
        float rateVal = this->r[s];

        float deltaX = rateVal <= 0.001f ? 1.0f : 1.0f / (rateVal * (float)this->sampleRate);
        this->m_phaseProgress += deltaX;

        bool phaseEnded = this->m_phaseProgress >= 1.0f;
        if (phaseEnded) {
            this->m_phaseProgress = 1.0f;
        }

        float y = m_curveCore->process(
            this->targetIndex,
            (int)CurveParams::Target::SsgSwPEnv11,
            doLoop ? (int)CurveParams::TargetSsgSwPEnv11::LoopTo : (s - 1),
            this->m_phaseProgress
        );
        this->currentLevel = startLevel + y * (targetLevel - startLevel);

        if (phaseEnded) {
            this->currentLevel = targetLevel;

            if (doLoop) {
                this->m_phaseProgress = 0.0f;
                this->state = (State)(this->loopTo + 1);
                if (this->loopCount > 0) this->loopCounter++;
            }
            else {
                if (this->steps > s) {
                    this->m_phaseProgress = 0.0f;
                    this->state = (State)(s + 1);
                }
            }
        }
    }

    // --- セント値を周波数比に変換して phaseDelta に適用 ---
    if (this->currentLevel != 0.0f) {
        // 1200セント = 1オクターブ (2倍の周波数)
        float pitchRatio = std::pow(2.0f, this->currentLevel / 1200.0f);
        phaseDelta *= pitchRatio;
    }

    return phaseDelta;
}
