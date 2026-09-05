#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct LfoOpnaParams {
	int syncDelay = CPV::OpnaLfo::SyncDelay::initial;
	bool pm = CPV::OpnaLfo::Vib::initial;
	bool am = CPV::OpnaLfo::Am::initial;
	int pmFreqIndex = CPV::OpnaLfo::Freqs::initial;
	int amFreqIndex = CPV::OpnaLfo::Freqs::initial;
	float pms = CPV::OpnaLfo::Pms::initial;
	float ams = CPV::OpnaLfo::Ams::initial;
	float amSmoothRate = CPV::OpnaLfo::AmSmRt::initial;
};