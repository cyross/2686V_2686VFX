#include "./Lfo.h"

void XbLfo::setParameters(const XbLfoParams& params)
{
	am.setParameters(params.am);
	pm.setParameters(params.pm);
}

void XbLfo::prepare(const LfoCounter* pCounter)
{
	this->counter = pCounter;

	am.prepare();
	pm.prepare();
}
