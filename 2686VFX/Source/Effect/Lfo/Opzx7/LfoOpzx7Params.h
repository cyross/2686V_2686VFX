#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct LfoOpzx7Params {
	bool pmEnable = CPV::Opzx7Lfo::Pm::initial;
	float pmFreq = CPV::Opzx7Lfo::PmFreq::initial;
	int pgIndex = CPV::Opzx7Lfo::PmShape::initial;
	int pmSyncDelay = CPV::Opzx7Lfo::SyncDelay::initial;
	float pms = CPV::Opzx7Lfo::Pms::initial;
	float pmd = CPV::Opzx7Lfo::Pmd::initial;

	bool amEnable = CPV::Opzx7Lfo::Am::initial;
	float amFreq = CPV::Opzx7Lfo::AmFreq::initial;
	int egIndex = CPV::Opzx7Lfo::AmShape::initial;
	int amSyncDelay = CPV::Opzx7Lfo::SyncDelay::initial;
	float amSmoothRate = CPV::Opzx7Lfo::AmSmRt::initial;
	float ams = CPV::Opzx7Lfo::Ams::initial;
	float amd = CPV::Opzx7Lfo::Amd::initial;
};