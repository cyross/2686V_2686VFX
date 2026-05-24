#include <algorithm>
#include <cmath>

#include "./Core.h"

PitchAdsrEnv::PitchAdsrEnv() {
	this->noteOnFunctions = std::array<std::function<void()>, 2>{
		[this]() { this->noteOnLinear(); },
		[this]() { this->noteOnCurve(); }
	};
	this->noteOffFunctions = std::array<std::function<void()>, 2>{
		[this]() { this->noteOffLinear(); },
		[this]() { this->noteOffCurve(); }
	};
	this->processFunctions = std::array<std::function<float(float)>, 2>{
		[this](float phaseDelta) { return this->processLinear(phaseDelta); },
		[this](float phaseDelta) { return this->processCurve(phaseDelta); }
	};
	this->bypassedReleasedProcessFunctions = std::array<std::function<float()>, 2>{
		[this]() { return this->bypassedReleasedProcessLinear(); },
		[this]() { return this->bypassedReleasedProcessCurve(); }
	};
	this->bypassedProcessFunctions = std::array<std::function<float()>, 2>{
		[this]() { return this->bypassedProcessLinear(); },
		[this]() { return this->bypassedProcessCurve(); }
	};
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
    if (this->m_curveCore == nullptr) {
        this->noteOnLinear();

        return;
    }

    this->noteOnFunctions[this->m_curveCore->index]();
}

void PitchAdsrEnv::noteOff() {
    if (this->m_curveCore == nullptr) {
        this->noteOffLinear();

        return;
    }

    this->noteOffFunctions[this->m_curveCore->index]();
}

float PitchAdsrEnv::process(float phaseDelta) {
    if (this->m_curveCore == nullptr) {
        return this->processLinear(phaseDelta);
    }

    return this->processFunctions[this->m_curveCore->index](phaseDelta);
}

float PitchAdsrEnv::bypassedReleasedProcess() {
    if (this->m_curveCore == nullptr) {
        return this->bypassedReleasedProcessLinear();
    }

    return this->bypassedReleasedProcessFunctions[this->m_curveCore->index]();
}

float PitchAdsrEnv::bypassedProcess() {
    if (this->m_curveCore == nullptr) {
        return this->bypassedProcessLinear();
    }

    return this->bypassedProcessFunctions[this->m_curveCore->index]();
}

void PitchAdsrEnv::noteOnLinear() {
    this->state = State::Attack;
    this->phaseProgress = 0.0f;
    this->currentCents = this->stl;
}

void PitchAdsrEnv::noteOffLinear() {
    this->state = State::Release;
    this->phaseProgress = 0.0f;
    // リリースフェーズは「現在のピッチ」からスタートするため、この瞬間の値を記録する
    this->releaseStartCents = this->currentCents;
}

float PitchAdsrEnv::processLinear(float phaseDelta) {
    if (this->bypass) {
        return phaseDelta;
    }

    // --- Pitch ADSR Logic (時間進行による補間) ---
    if (this->state == State::Attack) {
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
    }
    else if (this->state == State::Decay) {
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

    }
    else if (this->state == State::Sustain) {
        this->currentCents = this->ssl;
    }
    else if (this->state == State::Release) {
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
    }

    // --- セント値を周波数比に変換して phaseDelta に適用 ---
    if (this->state != State::Idle && this->currentCents != 0.0f) {
        // 1200セント = 1オクターブ (2倍の周波数)
        // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
        float pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
        return phaseDelta * pitchRatio;
    }

    return phaseDelta;
}

float PitchAdsrEnv::bypassedReleasedProcessLinear() {
    this->state = State::Idle;

    return 0.0f;
}

float PitchAdsrEnv::bypassedProcessLinear() {
    return 1.0f;
}

void PitchAdsrEnv::noteOnCurve() {
    this->state = State::Attack;
    this->phaseProgress = 0.0f;
    this->currentCents = this->stl;
}

void PitchAdsrEnv::noteOffCurve() {
    this->state = State::Release;
    this->phaseProgress = 0.0f;
    // リリースフェーズは「現在のピッチ」からスタートするため、この瞬間の値を記録する
    this->releaseStartCents = this->currentCents;
}

float PitchAdsrEnv::processCurve(float phaseDelta) {
    if (this->bypass) {
        return phaseDelta;
    }

    if (this->m_curveCore == nullptr) {
        return processLinear(phaseDelta);
    }

    int targetIdx = (int)CurveParams::Target::PitchEnv;
    // 今回のPitchAdsrEnvがオペレータ毎にあるのか共通なのかによってPositionを変える
    // ここでは共通(Common)またはオペレータごとの設定(this->targetIndex)を利用
    int posIdx = this->targetIndex;

    // --- Pitch ADSR Logic (カーブ進行による補間) ---
    if (this->state == State::Attack) {
        this->phaseProgress += this->attackDelta;

        if (this->phaseProgress >= 1.0f) {
            this->phaseProgress = 0.0f; // 次のフェーズ用にリセット
            this->currentCents = this->atl;
            this->state = State::Decay;
        }
        else {
            int prmIdx = (int)CurveParams::TargetPitchEnv::Ar;
            float y = m_curveCore->process(posIdx, targetIdx, prmIdx, this->phaseProgress);

            // stl から atl へ向かってカーブ補間
            this->currentCents = this->stl + (this->atl - this->stl) * y;
        }
    }
    else if (this->state == State::Decay) {
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
                int prmIdx = (int)CurveParams::TargetPitchEnv::Dr;
                float y = m_curveCore->process(posIdx, targetIdx, prmIdx, this->phaseProgress);

                // atl から ssl へ向かってカーブ補間
                this->currentCents = this->atl + (this->ssl - this->atl) * y;
            }
        }
    }
    else if (this->state == State::Sustain) {
        this->currentCents = this->ssl;
    }
    else if (this->state == State::Release) {
        this->phaseProgress += this->releaseDelta;

        if (this->phaseProgress >= 1.0f) {
            this->phaseProgress = 1.0f;
            this->currentCents = this->rll;
            this->state = State::Idle;
        }
        else {
            int prmIdx = (int)CurveParams::TargetPitchEnv::Rr;
            float y = m_curveCore->process(posIdx, targetIdx, prmIdx, this->phaseProgress);

            // キーを離した瞬間のピッチ (releaseStartCents) から rll へ向かってカーブ補間
            this->currentCents = this->releaseStartCents + (this->rll - this->releaseStartCents) * y;
        }
    }

    // --- セント値を周波数比に変換して phaseDelta に適用 ---
    if (this->state != State::Idle && this->currentCents != 0.0f) {
        // 1200セント = 1オクターブ (2倍の周波数)
        // 例: +1200 なら 2.0, -1200 なら 0.5, 0 なら 1.0 になる
        float pitchRatio = std::pow(2.0f, this->currentCents / 1200.0f);
        return phaseDelta * pitchRatio;
    }

    return phaseDelta;
}

float PitchAdsrEnv::bypassedReleasedProcessCurve() {
    this->state = State::Idle;

    return 0.0f;
}

float PitchAdsrEnv::bypassedProcessCurve() {
    return 1.0f;
}
