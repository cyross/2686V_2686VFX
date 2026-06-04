#include <algorithm>
#include <cmath>

#include "./Core.h"

PitchAdsrEnv::PitchAdsrEnv() {
}

void PitchAdsrEnv::prepare(int targetIndex, double sampleRate) {
    this->targetIndex = targetIndex;

    updateTargetSampleRate(sampleRate);
}

void PitchAdsrEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

void PitchAdsrEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
    this->updateIncrements();
}

void PitchAdsrEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    // 0秒に近い場合は即座に遷移(1.0)させる安全なラムダ関数
    auto calcDelta = [this](float timeSec) {
        return timeSec <= 0.001f ? 1.0f : 1.0f / (timeSec * this->sampleRate);
        };

    // 1サンプルあたりに進む「進行度（0.0〜1.0）」の割合を計算
    this->attackDelta = calcDelta(this->ar);
    this->decayDelta = calcDelta(this->dr);
    this->releaseDelta = calcDelta(this->rr);
}

void PitchAdsrEnv::setParameters(const PitchAdsrParams& params) {
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->stl = params.stl;
	this->atl = params.atl;
    this->ssl = params.ssl;
	this->rll = params.rll;
	this->bypass = params.bypass;
    this->updateIncrements();
}

void PitchAdsrEnv::noteOn() {
    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        this->state = State::Attack;
        this->phaseProgress = 0.0f;
        this->currentCents = this->stl;
    }
    else {
        this->state = State::Attack;
        this->phaseProgress = 0.0f;
        this->currentCents = this->stl;
    }
}

void PitchAdsrEnv::noteOff() {
    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        this->state = State::Release;
        this->phaseProgress = 0.0f;
        // リリースフェーズは「現在のピッチ」からスタートするため、この瞬間の値を記録する
        this->releaseStartCents = this->currentCents;
    }
    else {
        this->state = State::Release;
        this->phaseProgress = 0.0f;
        // リリースフェーズは「現在のピッチ」からスタートするため、この瞬間の値を記録する
        this->releaseStartCents = this->currentCents;
    }
}

float PitchAdsrEnv::process(float phaseDelta) {
    if (this->bypass) {
        return phaseDelta;
    }

    float pitchRatio = 0.0f;

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        switch (this->state) {
        case State::Idle:
            return phaseDelta;
        case State::Attack:
            this->phaseProgress += this->attackDelta;

            if (this->phaseProgress >= 1.0f) {
                this->phaseProgress = 0.0f; // 次のフェーズ用にリセット
                this->currentCents = this->atl;
                this->state = State::Decay;
            }
            else {
                // stl から atl へ向かって補間
                this->currentCents = this->stl + (this->atl - this->stl) * this->phaseProgress;
            }

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        case State::Decay:
            // DR(Decay Rate)が0の時は、減衰せずにATLを永遠に維持する
            if (this->dr <= 0.0f)
            {
                this->currentCents = this->atl;
            }
            else {
                this->phaseProgress += this->decayDelta;

                if (this->phaseProgress >= 1.0f) {
                    this->phaseProgress = 0.0f;
                    this->currentCents = this->ssl;
                    this->state = State::Sustain;
                }
                else {
                    // atl から ssl へ向かって補間
                    this->currentCents = this->atl + (this->ssl - this->atl) * this->phaseProgress;
                }
            }

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        case State::Sustain:
            this->currentCents = this->ssl;

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        case State::Release:
            this->phaseProgress += this->releaseDelta;

            if (this->phaseProgress >= 1.0f) {
                this->phaseProgress = 1.0f;
                this->currentCents = this->rll;
                this->state = State::Idle;
            }
            else {
                // キーを離した瞬間のピッチ (releaseStartCents) から rll へ向かって補間
                this->currentCents = this->releaseStartCents + (this->rll - this->releaseStartCents) * this->phaseProgress;
            }

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        }
    }
    else {
        switch (this->state) {
        case State::Idle:
            return phaseDelta;
        case State::Attack:
            this->phaseProgress += this->attackDelta;

            if (this->phaseProgress >= 1.0f) {
                this->phaseProgress = 0.0f; // 次のフェーズ用にリセット
                this->currentCents = this->atl;
                this->state = State::Decay;
            }
            else {
                float y = m_curveCore->process(
                    this->targetIndex,
                    (int)CurveParams::Target::PitchEnv,
                    (int)CurveParams::TargetPitchEnv::Ar,
                    this->phaseProgress
                );

                // stl から atl へ向かってカーブ補間
                this->currentCents = this->stl + (this->atl - this->stl) * y;
            }

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        case State::Decay:
            // DR(Decay Rate)が0の時は、減衰せずにATLを永遠に維持する
            if (this->dr <= 0.0f)
            {
                this->currentCents = this->atl;
            }
            else {
                this->phaseProgress += this->decayDelta;

                if (this->phaseProgress >= 1.0f) {
                    this->phaseProgress = 0.0f;
                    this->currentCents = this->ssl;
                    this->state = State::Sustain;
                }
                else {
                    float y = m_curveCore->process(
                        this->targetIndex,
                        (int)CurveParams::Target::PitchEnv,
                        (int)CurveParams::TargetPitchEnv::Dr,
                        this->phaseProgress
                    );

                    // atl から ssl へ向かってカーブ補間
                    this->currentCents = this->atl + (this->ssl - this->atl) * y;
                }
            }

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        case State::Sustain:
            this->currentCents = this->ssl;

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        case State::Release:
            this->phaseProgress += this->releaseDelta;

            if (this->phaseProgress >= 1.0f) {
                this->phaseProgress = 1.0f;
                this->currentCents = this->rll;
                this->state = State::Idle;
            }
            else {
                float y = m_curveCore->process(
                    this->targetIndex,
                    (int)CurveParams::Target::PitchEnv,
                    (int)CurveParams::TargetPitchEnv::Rr,
                    this->phaseProgress
                );

                // キーを離した瞬間のピッチ (releaseStartCents) から rll へ向かってカーブ補間
                this->currentCents = this->releaseStartCents + (this->rll - this->releaseStartCents) * y;
            }

            // --- セント値を周波数比に変換して phaseDelta に適用 ---
            if (this->currentCents != 0.0f) {
                // 1200セント = 1オクターブ (2倍の周波数)
                // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
                pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
                phaseDelta *= pitchRatio;
            }

            return phaseDelta;
        }
    }

    return phaseDelta;
}

float PitchAdsrEnv::bypassedReleasedProcess() {
    this->state = State::Idle;

    return 0.0f;
}

float PitchAdsrEnv::bypassedProcess() {
    return 1.0f;
}
