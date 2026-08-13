#include <algorithm>

#include "./EnvAmpAdsr.h"

AmpAdsrEnv::AmpAdsrEnv() {
}

void AmpAdsrEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void AmpAdsrEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
    this->updateIncrements();
}

void AmpAdsrEnv::setParameters(const AmpAdsrParams& params) {
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->stl = params.stl;
    this->kor = params.kor;
	this->bypass = params.bypass;
    this->updateIncrements();
}

void AmpAdsrEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    // 1. 純粋な時間進行度 (1サンプルあたりに 0.0 から 1.0 に向かうステップ量)
    this->attackTimeInc = 1.0f / (float)(std::max(0.00001f, this->ar) * this->sampleRate);
    this->decayTimeInc = 1.0f / (float)(std::max(0.00001f, this->dr) * this->sampleRate);
    this->releaseTimeInc = 1.0f / (float)(std::max(0.00001f, this->rr) * this->sampleRate);

    // 2. リニアモード用のレベル増減量
    this->attackInc = (1.0f - this->stl) * this->attackTimeInc;
    this->decayDec = (1.0f - this->sl) * this->decayTimeInc;
    this->releaseDec = this->kor ? 0.0f : this->sl * this->releaseTimeInc;
}

void AmpAdsrEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

float AmpAdsrEnv::noteOn() {
    if (this->bypass) {
        return 1.0f;
    }

    this->state = State::Attack;

    return this->stl;
}

void AmpAdsrEnv::noteOff() {
    if (this->bypass) {
        return;
    }

    this->state = State::Release;
    this->m_phaseProgress = 0.0f; // kor向け
}

float AmpAdsrEnv::bypassedReleasedProcess() {
    this->state = State::Idle;

    return 0.0f;
}

float AmpAdsrEnv::bypassedProcess() {
    return 1.0f;
}

float AmpAdsrEnv::process(float currentLevel) {
    if (this->bypass) {
        return 1.0f;
    }

    float limitLevel = 0.0f;

    switch (this->state) {
    case State::Idle:
        return currentLevel;
    case State::Attack:
        currentLevel += this->attackInc;

        if (currentLevel >= 1.0f) {
            currentLevel = 1.0f;
            this->state = State::Decay;
        }

        return currentLevel;
    case State::Decay:
        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (this->dr <= 0.0f)
        {
            currentLevel = 1.0f;
            this->state = State::Sustain;
        }
        else if (currentLevel > this->sl) {
            currentLevel -= this->decayDec;

            if (currentLevel <= (this->sl + 0.001f)) {
                currentLevel = this->sl;
                this->state = State::Sustain;
            }
        }
        else {
            currentLevel = this->sl;
            this->state = State::Sustain;
        }

        return currentLevel;
    case State::Sustain:
        return currentLevel;
    case State::Release:
        // kor向けに時間を進める
        this->m_phaseProgress += this->releaseTimeInc;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 0.0f;
            this->state = State::Idle;
            return 0.0f;
        }

        currentLevel -= this->releaseDec;

        if (currentLevel <= 0.001f) {
            currentLevel = 0.0f;
            this->state = State::Idle;
        }

        return currentLevel;
    }

    return currentLevel; // 念のため、どこにも当てはまらない場合は現在のレベルを維持
}
