#include "./GenNoiseSsg.h"

#include "Shared/Core/Processor/ProcessorValues.h"

void SsgNoiseGen::prepare(double newTargetRate) {
	this->lfsr = 0x1FFFF;
	this->currentSample = 0.0f;
	this->targetSampleRate = newTargetRate;
}

void SsgNoiseGen::setParameters(const SsgNoiseGenParams& params)
{
	this->level = params.level;
	this->baseFreq = params.baseFreq;
	this->noiseOnNote = params.noiseOnNote;
}

void SsgNoiseGen::generate() {
	if (this->targetFreq > 0.001f)
	{
		this->phase += this->delta;

		while (this->phase >= 1.0f)
		{
			this->phase -= 1.0f;

			if (this->fcMode) {
				this->currentSample = Tnd2A03::stepNoise(this->lfsrFc);
				continue;
			}

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
	// More Real FC は、進む速さを 2A03 の 16 段の表へ寄せる (N.FREQ の範囲で 16 段すべてに届く)
	const double rate = this->fcMode ? Tnd2A03::noiseRateFromSsg(this->targetFreq, (double)CPV::Tn::Freq::max) : (double)this->targetFreq;

	if (this->targetSampleRate > 0.0) this->delta = (float)(rate / this->targetSampleRate);
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
