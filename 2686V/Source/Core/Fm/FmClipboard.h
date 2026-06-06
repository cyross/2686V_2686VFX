#pragma once

#include <array>

#include "../Synth/SynthMode.h"

// FM音源のパラメータをコピー・ペーストするために使用
struct FmOpClipboard
{
	bool rgMode;
	int rar;
	int rdr;
	int rsr;
	int rrr;
	int rsl;
	int rtl;
	float ar;
	float dr;
	float sr;
	float rr;
	float sl;
	float tl;
	bool ksr;
	int ksl;
	int mul;
	int dt;
	int dt2;
};

// FM音源のパラメータをコピー・ペーストするために使用
struct FmClipboard
{
	OscMode mode = OscMode::OPNA; // FM音源モード

	int ops = 4; // オペレータ数

	std::array<FmOpClipboard, 4> op; // オペレータ毎のパラメータ

	int alg = 0; // アルゴリズム
	int fb1 = 0; // フィードバック(OP1)
	int fb3 = 0; // フィードバック(OP3)
};
