#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct OpmDetuneParams
{
	int detune = CPV::OpmDetune::Dt::initial;
	int detune2 = CPV::OpmDetune::Dt2::initial;
	int multiple = CPV::OpmDetune::Mul::initial;
	float mulRatio = CPV::OpmDetune::MulRatio::initial;
};
