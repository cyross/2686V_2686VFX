#include "./Core.h"

void FixMode::setParameters(const FixModeParams& params)
{
	this->enable = params.enable;
	this->freq = params.freq;
}
