#include "./Core.h"

void LfsrNoiseGen::prepare(double newTargetRate) {
	this->lfsr = 0x1FFFF;
	this->currentSample = 0.0f;

	updateDelta(newTargetRate);
}

void LfsrNoiseGen::setParameters(float newLevel, float newBaseFreq, bool noiseOnNote)
{
	this->level = newLevel;
	this->baseFreq = newBaseFreq;
	this->noiseOnNote = noiseOnNote;
}

float LfsrNoiseGen::generate() {
	// LFSRの更新
	unsigned int bit0 = this->lfsr & 1;
	unsigned int bit3 = (this->lfsr >> 3) & 1;
	unsigned int nextBit = bit0 ^ bit3;

	this->lfsr >>= 1;
	if (nextBit) this->lfsr |= (1 << 16);

	// ノイズサンプルの生成 (例: -1.0f 〜 +1.0f)
	this->currentSample = ((this->lfsr % 1000) / 500.0f) - 1.0f;

	return this->currentSample;
}

void LfsrNoiseGen::updateDelta(double newTargetRate) {
	this->targetSampleRate = newTargetRate;

	if (this->targetSampleRate > 0.0) this->delta = this->targetFreq / this->targetSampleRate;
}

void LfsrNoiseGen::updateFrequency(float currentFreq)
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
