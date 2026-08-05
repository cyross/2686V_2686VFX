#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct OpnDetuneParams
{
	int detune = CPV::OpnDetune::Dt::initial;
	int multiple = CPV::OpnDetune::Mul::initial;
};
