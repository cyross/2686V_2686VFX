#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct Opzx7DetuneParams
{
	int detune = CPV::Opzx7Detune::Dt1::initial;
	int detune2 = CPV::Opzx7Detune::Dt2::initial;
	int detune3 = CPV::Opzx7Detune::Dt3::initial;
	int multiple = CPV::Opzx7Detune::Mul::initial;
	float mulRatio = CPV::Opzx7Detune::MulRatio::initial;
};
