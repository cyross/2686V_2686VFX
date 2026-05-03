#include "./Core.h"
#include "../../Synth/SynthHelpers.h"

void FmSampleCore::prepare(double sampleRate) {
	if (sampleRate > 0.0) this->hostSampleRate = sampleRate;

	this->targetSampleRate = SynthHelpers::getTargetRate(this->rateIndex);
	this->stepSize = this->targetSampleRate / this->hostSampleRate;

	this->rateAcc = 1.0;
}

void FmSampleCore::setParameters(int newRateIndex) {
	this->rateIndex = newRateIndex;
	this->targetSampleRate = SynthHelpers::getTargetRate(this->rateIndex);
	this->stepSize = this->targetSampleRate / this->hostSampleRate;
}

void FmSampleCore::noteOn() {
	this->rateAcc = 1.0;
}

void FmSampleCore::updateAcc() {
	this->rateAcc += this->stepSize;
}

bool FmSampleCore::isActive() const {
	return this->rateAcc >= 1.0;
}

void FmSampleCore::start() {
	this->rateAcc -= 1.0;
	this->prevSample = this->lastSample;
}

void FmSampleCore::finish(float finalSample) {
	this->lastSample = finalSample;
}

float FmSampleCore::getSample() const {
	double fraction = this->rateAcc / this->stepSize;

	if (fraction > 1.0)
		fraction = 1.0;

	return this->prevSample + (this->lastSample - this->prevSample) * fraction;
}
