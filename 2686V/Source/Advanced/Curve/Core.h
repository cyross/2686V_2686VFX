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
    static constexpr int LUT_SIZE = 1024;
    std::array<std::array<std::array<std::array<float, LUT_SIZE>, 8>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> curveLUT;

    // 内部の生計算関数
    float processRaw(int positionIndex, int targetIndex, int paramIndex, float x);
public:
    CurveCore();

    int index = 0; // ロジック切り替えのインデックス

	void setParameters(const CurveParams& params);
    void bakeCurves();
    void bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex);
    float process(int positionIndex, int targetIndex, int paramIndex, float x); // x: 正規化入力値(0.0f ~ 1.0f)
	std::function<float(int, int, int, float)> getFunction(int logicIndex);
};
