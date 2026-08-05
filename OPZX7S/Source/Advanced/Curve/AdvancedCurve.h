#pragma once

#include <JuceHeader.h>

#include "./AdvancedCurveParams.h"
#include "../../Processor/Curve/ProcessorCurveValues.h"

class CurveCore
{
private:
	CurveParams m_params;

	std::map<CurveParams::Logic, std::function<float(int, int, int, float)>> logics; // ロジックごとの関数マップ
public:
    CurveCore();

    int index = 0; // ロジック切り替えのインデックス

	void setParameters(const CurveParams& params);
    void bakeCurves() {}
    void bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex) {}
    float processRaw(int positionIndex, int targetIndex, int paramIndex, float x) const;
    inline float process(int positionIndex, int targetIndex, int paramIndex, float x) const noexcept { // x: 正規化入力値(0.0f ~ 1.0f)
        float safeX = std::clamp(x, 0.0f, 1.0f);
        if (std::isnan(safeX)) return 0.0f;

        // LUT配列を参照せず、直接計算する（最新のCPUではこの方が安全で速い）
        float result = processRaw(positionIndex, targetIndex, paramIndex, safeX);

        if (std::isnan(result) || std::isinf(result)) return safeX; // フェイルセーフ

        return std::clamp(result, 0.0f, 1.0f);
    };
};
