#include "./Noise.h"

void LfsrNoiseGen::setup() {
	lfsr = 0x1FFFF; // 17-bit LFSR初期値
	currentSample = 0.0f;
}

float LfsrNoiseGen::generateSample() {
	// LFSRの更新
	unsigned int bit0 = lfsr & 1;
	unsigned int bit3 = (lfsr >> 3) & 1;
	unsigned int nextBit = bit0 ^ bit3;
	lfsr >>= 1;
	if (nextBit) lfsr |= (1 << 16);
	// ノイズサンプルの生成 (例: -1.0f 〜 +1.0f)
	currentSample = ((lfsr % 1000) / 500.0f) - 1.0f;

	return currentSample;
}

void LfsrNoiseGen::updateNoiseDelta(double targetRate) {
	if (targetRate > 0.0) delta = targetFreq / targetRate;
}
