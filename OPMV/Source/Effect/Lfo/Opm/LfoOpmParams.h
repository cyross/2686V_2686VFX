#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct LfoOpmParams {
	int syncDelay = CPV::OpmLfo::SyncDelay::initial;
	bool pm = CPV::OpmLfo::Pm::initial;
	bool am = CPV::OpmLfo::Am::initial;
	float pmFreq = CPV::OpmLfo::Freq::initial;
	float amFreq = CPV::OpmLfo::Freq::initial;
	int pgIndex = CPV::OpmLfo::PgShape::initial;
	int egIndex = CPV::OpmLfo::EgShape::initial;
	int pmsIndex = CPV::OpmLfo::Pms::initial;
	int pmd = CPV::OpmLfo::Pmd::initial;
	int amsIndex = CPV::OpmLfo::Ams::initial;
	int amd = CPV::OpmLfo::Amd::initial;
	float amSmoothRate = CPV::OpmLfo::AmSmRt::initial;
};

struct LfoOpmOpParams {
	bool amsEnable = CPV::OpmLfo::AmsEn::initial;
};
