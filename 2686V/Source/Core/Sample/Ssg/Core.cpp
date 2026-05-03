#include "./Core.h"
#include "../../Synth/SynthHelpers.h"

void SsgSampleCore::prepare(double sampleRate) {
	if (sampleRate > 0.0) this->hostSampleRate = sampleRate;

	this->targetSampleRate = SynthHelpers::getTargetRate(this->rateIndex);
	this->stepSize = this->targetSampleRate / this->hostSampleRate;

	this->rateAcc = 1.0;
}

void SsgSampleCore::setParameters(int newRateIndex) {
	this->rateIndex = newRateIndex;
	this->targetSampleRate = SynthHelpers::getTargetRate(this->rateIndex);
	this->stepSize = this->targetSampleRate / this->hostSampleRate;
}

void SsgSampleCore::noteOn() {
	this->currentLevel = 0.0f;
	this->lastLevel = 0.0f;
	this->rateAcc = 1.0;
}

void SsgSampleCore::preProcess() {
	this->rateAcc += this->stepSize;
	this->steps = 0;
	this->sumOut = 0.0f;
}

bool SsgSampleCore::isActive() {
	return this->rateAcc >= 1.0;
}

void SsgSampleCore::start() {
	this->rateAcc -= 1.0;
}

void SsgSampleCore::finish(float finalLevel) {
	this->sumOut += finalLevel;
	this->steps++;
}

float SsgSampleCore::getSample() {
	if (this->steps > 0) {
		this->lastLevel = this->sumOut / (float)this->steps;
	}

	return this->lastLevel * this->currentLevel * 0.5f;
}
