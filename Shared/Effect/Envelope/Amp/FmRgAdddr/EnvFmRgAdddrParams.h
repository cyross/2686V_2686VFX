#pragma once

#include "Shared/Core/Synth/LivePosition.h"

#include "../../../KeyScale/Opn/KSOpnParams.h"
#include "../../../KeyScale/Opp/KSOppParams.h"

enum class FmRgAdddrKeyScaleMode {
	OPM = 0,
	OPP = 1
};

struct FmRgAdddrParams
{
	// 鳴っている位置を書く先 (グラフの縦線。4.3.0)。nullptr なら書かない
	LivePosition* live = nullptr;

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