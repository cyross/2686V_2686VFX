#include "./Unit.h"

void OpnaLfoUnit::setParameters(const OpnaLfoUnitParams& params)
{
	enable = params.enable;
	freq = params.freq;
	freqIndex = params.freqIndex;
	wave = params.wave;
	sensitivity = params.sensitivity;
	egLfoWave = params.egLfoWave;
	smoothRate = params.smoothRate;
}

void OpnaLfoUnit::prepare()
{
	timerAcc = 1.0f;
	steppedVal = 0.0f;
	smooth = 0.0f;
}
