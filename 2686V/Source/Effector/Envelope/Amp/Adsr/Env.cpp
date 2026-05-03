#include <algorithm>

#include "./Env.h"

AmpAdsrEnv::AmpAdsrEnv(float ar, float dr, float sl, float rr, float tl)
{
	this->ar = ar;
	this->dr = dr;
	this->sl = sl;
	this->rr = rr;
	this->tl = tl;
}

void AmpAdsrEnv::setParameters(const AmpAdsrEnvParams& params, bool bypass)
{
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->tl = params.tl;
	this->bypass = bypass;
	this->updateIncrements();
}

void AmpAdsrEnv::updateIncrements()
{
	if (this->sampleRate <= 0.0) return;

	this->attackInc = 1.0f / (float)(std::max(0.001f, this->ar) * this->sampleRate);
	this->decayDec = 1.0f / (float)(std::max(0.001f, this->dr) * this->sampleRate);
	this->releaseDec = 1.0f / (float)(std::max(0.001f, this->rr) * this->sampleRate);
}

void AmpAdsrEnv::updateIncrements(double newSampleRate)
{
	if (newSampleRate <= 0.0) return;

	this->sampleRate = newSampleRate;

	this->attackInc = 1.0f / (float)(std::max(0.001f, this->ar) * this->sampleRate);
	this->decayDec = 1.0f / (float)(std::max(0.001f, this->dr) * this->sampleRate);
	this->releaseDec = 1.0f / (float)(std::max(0.001f, this->rr) * this->sampleRate);
}

float AmpAdsrEnv::process(float currentLevel)
{
	if (this->state == State::Attack) {
		currentLevel += this->attackInc;

		if (currentLevel >= 1.0f) { currentLevel = 1.0f; this->state = State::Decay; }
	}
	else if (this->state == State::Decay) {
		if (currentLevel > this->sl) {
			currentLevel -= this->decayDec;

			if (currentLevel <= this->sl) { currentLevel = this->sl; this->state = State::Sustain; }
		}
		else { currentLevel = this->sl; this->state = State::Sustain; }
	}
	else if (this->state == State::Sustain) {
		currentLevel = this->sl;
	}
	else if (this->state == State::Release) {
		currentLevel -= this->releaseDec;

		if (currentLevel <= 0.0f) { currentLevel = 0.0f; this->state = State::Idle; }
	}

	return currentLevel;
}
