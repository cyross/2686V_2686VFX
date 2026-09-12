#pragma once

class PitchAdsrParams {
public:
	float ar = 0.0f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.0f;
	int stl = 0;
	int atl = 0;
	int ssl = 0;
	int rll = 0;

	// リリースを走り終えたあとに保つセント。0 でこれまでどおり。
	int endl = 0;

	// ENDL を使うかどうか。切のあいだはこれまでどおり。
	bool endlEnable = false;

	// 段ごとのレベルを斜めに繋がず、その段のあいだ保ち続ける。
	bool keep = false;

	bool bypass = false;
};
