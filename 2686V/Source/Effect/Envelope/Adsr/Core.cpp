#include <algorithm>

#include "./Core.h"

void AdsrAmpEnv::prepare(double sampleRate) {
	this->sampleRate = sampleRate;
    this->updateIncrements();
}

void AdsrAmpEnv::setParameters(const AdsrParams& params) {
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->bypass = params.bypass;
    this->updateIncrements();
}

void AdsrAmpEnv::noteOn() {
    this->state = State::Attack;
}

void AdsrAmpEnv::noteOff() {
    this->state = State::Release;
}

float AdsrAmpEnv::bypassedReleasedProcess() {
   this->state = State::Idle;

   return 0.0f;
}

float AdsrAmpEnv::bypassedProcess() {
    return 1.0f;
}

float AdsrAmpEnv::process(float currentLevel) {
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
        if (newLevel > this->sl) {
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

void AdsrAmpEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    this->attackInc = 1.0f / (float)(std::max(0.001f, this->ar) * this->sampleRate);
    this->decayDec = 1.0f / (float)(std::max(0.001f, this->dr) * this->sampleRate);
    this->releaseDec = 1.0f / (float)(std::max(0.001f, this->rr) * this->sampleRate);
}

void AdsrAmpEnv::updateSampleRate(double newSampleRate) {
	this->sampleRate = newSampleRate;
    this->updateIncrements();
}