#include "./FmFix.h"

void FixMode::setParameters(bool enable, float freq)
{
	this->enable = enable;
	this->freq = freq;
}

float FixMode::noteOn(float baseFreq)
{
	return enable ? freq : baseFreq;
}
