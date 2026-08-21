#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct LfoOplParams {
	bool pm = CPV::OplLfo::Vib::initial;
	bool am = CPV::OplLfo::Am::initial;
	float pms = CPV::OplLfo::Pms::initial;
	float pmd = CPV::OplLfo::Pmd::initial;
	float ams = CPV::OplLfo::Ams::initial;
	float amd = CPV::OplLfo::Amd::initial;
};
