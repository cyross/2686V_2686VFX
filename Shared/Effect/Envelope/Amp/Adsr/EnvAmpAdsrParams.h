#pragma once

#include "Shared/Core/Synth/LivePosition.h"

class AmpAdsrParams {
public:
	// 鳴っている位置を書く先 (グラフの縦線。4.3.0)。nullptr なら書かない
	LivePosition* live = nullptr;

	float stl = 0.0f;
	float ar = 0.01f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.2f;
	bool kor = false;

	// リリースを走り終えたあとに保つレベル。0.0 でこれまでどおり。
	float endl = 0.0f;

	// ENDL を使うかどうか。切のあいだはこれまでどおり。
	bool endlEnable = false;

	bool bypass = false;
};
