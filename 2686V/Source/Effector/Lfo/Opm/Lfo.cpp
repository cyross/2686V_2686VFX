#include "./Lfo.h"

void OpmLfo::setParameters(const OpmLfoParams& params)
{
	am.setParameters(params.am);
	pm.setParameters(params.pm);
}

void OpmLfo::prepare(const LfoCounter* pCounter)
{
	this->counter = pCounter;

	am.prepare();
	pm.prepare();
}
