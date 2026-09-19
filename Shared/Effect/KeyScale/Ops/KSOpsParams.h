#pragma once

#include "../../../Core/Processor/ProcessorValues.h"

struct KSOpsParams {
	int ksBp = CPV::Opzx7Ks::KsBp::initial;
	int ksLc = CPV::Opzx7Ks::KsLc::initial;
	int ksRc = CPV::Opzx7Ks::KsRc::initial;
	float ksLd = CPV::Opzx7Ks::KsLd::initial;
	float ksRd = CPV::Opzx7Ks::KsRd::initial;
	int ksRs = CPV::Opzx7Ks::KsRs::initial;
};
