#pragma once

#include <JuceHeader.h>

#include "./Params.h"

class CurveCore
{
private:
	CurveParams m_params;

	std::map<CurveParams::Logic, std::function<float(int, int, int, float)>> logics; // ロジックごとの関数マップ
public:
    CurveCore();

    int index = 0; // ロジック切り替えのインデックス

	void setParameters(const CurveParams& params);
	float process(int positionIndex, int targetIndex, int paramIndex, float x); // x: 正規化入力値(0.0f ~ 1.0f)
	std::function<float(int, int, int, float)> getFunction(int logicIndex);
};
