#pragma once

#include <JuceHeader.h>

#include "./Params.h"

class CurveCore
{
private:
	CurveParams m_params;

	std::map<CurveParams::Logic, std::function<float(int, int, int, float)>> logics; // ロジックごとの関数マップ

    // カーブの結果を保存する配列 (解像度 1024)
    // [position][target][param][1024 steps]
    static constexpr int LUT_SIZE = 2048;
    float curveLUT[5][4][8][LUT_SIZE] = { 0.0f };

    // 内部の生計算関数
    float processRaw(int positionIndex, int targetIndex, int paramIndex, float x);
public:
    CurveCore();

    int index = 0; // ロジック切り替えのインデックス

	void setParameters(const CurveParams& params);
    void bakeCurves();
    void bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex);
    inline float process(int positionIndex, int targetIndex, int paramIndex, float x) const noexcept { // x: 正規化入力値(0.0f ~ 1.0f)
		// 極微小な誤差による破綻を防ぐためのガード
		// (これがないと、スプライン計算などで x=0 のときに意図せず 1.0 を返したりする場合があります)
		if (x <= 1e-5f) return 0.0f;
		if (x >= 1.0f - 1e-5f) return 1.0f;

		float fIndex = x * (LUT_SIZE - 1);
		int i = (int)fIndex;

		// (ここでは高速化のため単純な切り捨て配列参照にする)
		return curveLUT[positionIndex][targetIndex][paramIndex][i];
    };
	std::function<float(int, int, int, float)> getFunction(int logicIndex);
};
