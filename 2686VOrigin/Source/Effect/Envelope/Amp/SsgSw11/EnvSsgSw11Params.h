#pragma once

class SsgSwEnv11Params {
public:
	int steps = 5;
	bool loop = false;
	int loopTo = 0;
	int loopCount = 0;
	float stl = 0.0f;
	float r1 = 0.0f;
	float l1 = 0.0f;
	float r2 = 0.0f;
	float l2 = 0.0f;
	float r3 = 0.0f;
	float l3 = 0.0f;
	float r4 = 0.0f;
	float l4 = 0.0f;
	float r5 = 0.0f;
	float l5 = 0.0f;
	float r6 = 0.0f;
	float l6 = 0.0f;
	float r7 = 0.0f;
	float l7 = 0.0f;
	float r8 = 0.0f;
	float l8 = 0.0f;
	float r9 = 0.0f;
	float l9 = 0.0f;
	float r10 = 0.0f;
	float l10 = 0.0f;
	float r11 = 0.0f;
	float l11 = 0.0f;

	// リリースを走り終えたあとに保つレベル。0.0 でこれまでどおり。
	float endl = 0.0f;

	// ENDL を使うかどうか。切のあいだはこれまでどおり。
	bool endlEnable = false;

	// 段ごとのレベルを斜めに繋がず、その段のあいだ保ち続ける。
	bool keep = false;

	bool bypass = false;
};
