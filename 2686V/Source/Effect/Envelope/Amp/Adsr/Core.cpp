#include <algorithm>

#include "./Core.h"

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
	this->bypass = params.bypass;
    this->updateIncrements();
}

void AmpAdsrEnv::noteOn() {
    this->state = State::Attack;
}

void AmpAdsrEnv::noteOff() {
    this->state = State::Release;
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
        return currentLevel;
    }

	float newLevel = currentLevel;

    // --- ADSR Logic ---
    if (this->state == State::Attack) {
        newLevel += this->attackInc;

        if (newLevel >= 1.0f) {
            newLevel = 1.0f;
            this->state = State::Decay;
        }
    }
    else if (this->state == State::Decay) {
        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (this->dr <= 0.0f)
        {
            newLevel = 1.0f;
            this->state = State::Sustain;
        }
        else if (newLevel > this->sl) {
            newLevel -= this->decayDec;

            if (newLevel <= this->sl) {
                newLevel = this->sl;
                this->state = State::Sustain;
            }
        }
        else {
            newLevel = this->sl;
            this->state = State::Sustain;
        }
    }
    else if (this->state == State::Release) {
        newLevel -= this->releaseDec;

        if (newLevel <= 0.0f) {
            newLevel = 0.0f;
            this->state = State::Idle;
        }
    }

    return newLevel;
}

void AmpAdsrEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    this->attackInc = 1.0f / (float)(std::max(0.001f, this->ar) * this->sampleRate);
    this->decayDec = 1.0f / (float)(std::max(0.001f, this->dr) * this->sampleRate);
    this->releaseDec = 1.0f / (float)(std::max(0.001f, this->rr) * this->sampleRate);
}

void AmpAdsrEnv::updateSampleRate(double newSampleRate) {
	this->sampleRate = newSampleRate;
    this->updateIncrements();
}
