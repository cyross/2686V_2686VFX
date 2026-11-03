#pragma once

#include "Shared/Core/Synth/LivePosition.h"

#include "../../../KeyScale/Opl/KSOplParams.h"

struct OplAdsrParams
{
	// 鳴っている位置を書く先 (グラフの縦線。4.3.0)。nullptr なら書かない
	LivePosition* live = nullptr;

	int ar = 0;
	int dr = 0;
	int sl = 15;
	int rr = 0;
	int tl = 0;

	KSOplParams ksOPL;

	bool egType = false;
	bool sus = false;
	bool xof = false;
	bool kor = false;
	bool bypass = false;
};