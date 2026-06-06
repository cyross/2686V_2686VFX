#include "./GenNoiseSsg.h"

void SsgNoiseGen::prepare(double newTargetRate) {
	this->lfsr = 0x1FFFF;
	this->currentSample = 0.0f;
	this->targetSampleRate = newTargetRate;
}

void SsgNoiseGen::setParameters(float newLevel, float newBaseFreq, bool noiseOnNote)
{
	this->level = newLevel;
	this->baseFreq = newBaseFreq;
	this->noiseOnNote = noiseOnNote;
}

void SsgNoiseGen::generate() {
	if (this->targetFreq > 0.001f)
	{
		this->phase += this->delta;

		while (this->phase >= 1.0f)
		{
			this->phase -= 1.0f;

			// LFSRの更新
			unsigned int bit0 = this->lfsr & 1;
			unsigned int bit3 = (this->lfsr >> 3) & 1;
			unsigned int nextBit = bit0 ^ bit3;

			this->lfsr >>= 1;

			if (nextBit) this->lfsr |= (1 << 16);

			this->currentSample = (this->lfsr & 1) ? 1.0f : -1.0f;
		}
	}
	else {
		this->currentSample = 0.0f;
	}
}

void SsgNoiseGen::updateDelta() {
	if (this->targetSampleRate > 0.0) this->delta = this->targetFreq / this->targetSampleRate;
}

void SsgNoiseGen::updateFrequency(float currentFreq)
{
	if (this->noiseOnNote) {
		// ONの時：キーボードの音程に合わせてノイズの周波数を上下させる
		this->targetFreq = this->baseFreq * (currentFreq / 440.0);
	}
	else {
		// OFFの時：スライダーで設定したノイズ周波数に固定する
		this->targetFreq = baseFreq;
	}
}

float SsgNoiseGen::generateSample(float gain) {
	return this->currentSample * this->level * gain;
}
