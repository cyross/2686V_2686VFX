#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct LfoN88Params {
	int syncDelay = CPV::N88Lfo::SyncDelay::initial;

	bool pm = CPV::N88Lfo::Pm::initial;
	int pmIndex = CPV::N88Lfo::Shape::initial;
	float pmFreq = CPV::N88Lfo::Freq::initial;
	float pms = CPV::N88Lfo::Pms::initial;
	float pmd = CPV::N88Lfo::Pmd::initial;

	bool am = CPV::N88Lfo::Am::initial;
	int amIndex = CPV::N88Lfo::Shape::initial;
	float amFreq = CPV::N88Lfo::Freq::initial;
	float amSmoothRate = CPV::N88Lfo::AmSmRt::initial;
	float amd = CPV::N88Lfo::Amd::initial;
};

struct LfoN88OpParams {
	float ams = CPV::N88Lfo::Ams::initial;
};
