#include "./FmFix.h"

void FixMode::setParameters(const FixModeParams& params)
{
	this->enable = params.enable;
	this->freq = params.freq;
}

float FixMode::noteOn(float baseFreq)
{
	return enable ? freq : baseFreq;
}
