#include "./Lfo.h"

void GlobalLfo::setParameters(const GlobalLfoParams& params)
{
	am.setParameters(params.am);
	pm.setParameters(params.pm);
}

void GlobalLfo::prepare(const LfoCounter* pCounter)
{
	this->counter = pCounter;

	am.prepare();
	pm.prepare();
}
