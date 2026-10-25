#pragma once

#include "Shared/Core/Synth/LivePosition.h"

#include "../../../KeyScale/Opn/KSOpnParams.h"

struct FmRgAdssrParams
{
	// 鳴っている位置を書く先 (グラフの縦線。4.3.0)。nullptr なら書かない
	LivePosition* live = nullptr;

	int ar = 0;
	int dr = 0;
	int sr = 0;
	int sl = 15;
	int rr = 0;
	int tl = 0;

	KSOpnParams ksOPN;

	bool xof = false;
	bool kor = false;
	bool bypass = false;
};