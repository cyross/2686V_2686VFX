#include "./Lfo.h"

void N88Lfo::setParameters(const N88LfoParams& params)
{
	am.setParameters(params.am);
	pm.setParameters(params.pm);
}

void N88Lfo::prepare(const LfoCounter* pCounter)
{
	this->counter = pCounter;

	am.prepare();
	pm.prepare();
}
