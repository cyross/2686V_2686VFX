#include "./Lfo.h"

void OpnaLfo::setParameters(const OpnaLfoParams& params)
{
	am.setParameters(params.am);
	pm.setParameters(params.pm);
}

void OpnaLfo::prepare(const LfoCounter* pCounter)
{
	this->counter = pCounter;

	am.prepare();
	pm.prepare();
}
