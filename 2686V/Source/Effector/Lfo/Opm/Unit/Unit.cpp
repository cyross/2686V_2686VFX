#include "./Unit.h"

void OpmLfoUnit::setParameters(const OpmLfoUnitParams& params)
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

void OpmLfoUnit::prepare()
{
	phase = 0.0f;
	timerAcc = 1.0f;
	steppedVal = 0.0f;
	smooth = 0.0f;
}
