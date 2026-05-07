#pragma once

#include <array>

#include "../Synth/Mode.h"

// FM音源のパラメータをクリップボードにコピー・ペーストするために使用
struct FmOpClipboard
{
	int alg = 0; // アルゴリズム
	int fb1 = 0; // フィードバック(OP1)
	int fb3 = 0; // フィードバック(OP3)
};

// FM音源のパラメータをクリップボードにコピー・ペーストするために使用
struct FmClipboard
{
	OscMode mode = OscMode::OPNA; // FM音源モード

	int ops = 4; // オペレータ数

	std::array<FmOpClipboard, 4> op; // オペレータ毎のパラメータ
};
