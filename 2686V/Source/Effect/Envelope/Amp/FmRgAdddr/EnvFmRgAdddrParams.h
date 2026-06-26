#pragma once

#include "../../../KeyScale/Opn/KSOpnParams.h"
#include "../../../KeyScale/Opp/KSOppParams.h"

enum class FmRgAdddrKeyScaleMode {
	OPM = 0,
	OPP = 1
};

struct FmRgAdddrParams
{
	int ar = 0;
	int d1r = 0;
	int d2r = 0;
	int d1l = 15;
	int rr = 0;
	int tl = 0;

	FmRgAdddrKeyScaleMode ksMode = FmRgAdddrKeyScaleMode::OPM;
	KSOpnParams ksOPM;
	KSOppParams ksOPP;

	bool xof = false;
	bool kor = false;
	bool bypass = false;
};