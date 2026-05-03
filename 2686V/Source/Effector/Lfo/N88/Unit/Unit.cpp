#include "./Unit.h"

void N88LfoUnit::setParameters(const N88LfoUnitParams& params)
{
	enable = params.enable;
	freq = params.freq;
	freqIndex = params.freqIndex;
	wave = params.wave;
	sensitivity = params.sensitivity;
	depth = params.depth;
	egLfoWave = params.egLfoWave;
	smoothRate = params.smoothRate;
	phase = 0.0f; // Reset phase on setup
}

void N88LfoUnit::prepare()
{
	phase = 0.0f;
	timerAcc = 1.0f;
	steppedVal = 0.0f;
	smooth = 0.0f;
}
