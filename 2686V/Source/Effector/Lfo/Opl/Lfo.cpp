#include "./Lfo.h"

void OplLfo::setParameters(const OplLfoParams& params)
{
	am.setParameters(params.am);
	pm.setParameters(params.pm);
}

void OplLfo::prepare(const LfoCounter* pCounter)
{
	this->counter = pCounter;

	am.prepare();
	pm.prepare();
}
