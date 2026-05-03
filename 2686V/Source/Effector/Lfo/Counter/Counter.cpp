#include "./Counter.h"

void LfoCounter::prepare(double newSampleRate) {
	this->timerAcc = 0.0;
	this->targetSampleRate = newSampleRate;
}

void LfoCounter::setParameters(float syncDelay) {
	this->syncDelay = syncDelay;
}

void LfoCounter::noteOn() {
    if (this->isDelayActive()) {
        this->delayCounter = this->syncDelay / 1000.0f; // ms -> 秒
    }
    else {
        this->delayCounter = 0.0f; // フリーラン継続
    }

    this->cycleCount = 0;
}

bool LfoCounter::isActive() const {
    return this->delayCounter > 0.0f;
}

void LfoCounter::updateCounter() {
    this->delayCounter -= 1.0f / this->targetSampleRate;

    if (this->delayCounter < 0.0f) this->delayCounter = 0.0f;
}

void LfoCounter::updateTimer() {
    double timerInc = this->timerFreq / this->targetSampleRate; // 1サンプルあたりに進む60Hzタイマーの割合

    this->timerAcc += timerInc;
}

void LfoCounter::updatePhase() {
    this->timerAcc -= 1.0;

    // timerFreq(Hz) を timerFreq(Hz) で割ることで、1回の割り込みあたりの位相増分になる
    this->phase += (this->timerFreq / this->timerFreq);

}