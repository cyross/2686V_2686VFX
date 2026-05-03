#pragma once

#include <random>

class LfoCounter
{
	float timerFreq = 60.0f; // Hz
	float syncDelay = 0.0f; // ms
	float delayCounter = 0.0f; // seconds
	double targetSampleRate = 44100.0f;
public:
	uint32_t cycleCount = 0;
	double timerAcc = 0.0;
	void prepare(double newSampleRate);
	void setParameters(float syncDelay);
	void setSampleRate(double newSampleRate) { this->targetSampleRate = newSampleRate; }
	void noteOn();
	bool isDelayActive() const { return delayCounter > 0.0f; }
	bool isActive() const;
	void updateCounter();
	void updateTimer();
	bool isActiveTimer() const { return timerAcc >= 1.0; }
	void updatePhase();
};
