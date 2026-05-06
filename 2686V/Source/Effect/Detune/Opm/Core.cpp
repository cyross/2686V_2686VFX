#include "./Core.h"

// 実機(YM2151/2608)の挙動を模倣するため、定数加算ではなく周波数比例させます。
// これにより「キーによって周波数値が変わる（高音ほど変化Hzが大きい）」挙動になります。
// 値は実機の数値を参考に調整した近似値です。
// 0: 0
// 1: -0.45%
// 2: -0.25%
// 3: -0.1% (approx)
// 4: 0
// 5: +0.1% (approx)
// 6: +0.25%
// 7: +0.45%
const std::array<float, 8> OpmDetune::dtScales = { 0.0f, -0.0045f, -0.0025f, -0.001f, 0.0f, 0.001f, 0.0025f, 0.0045f };

// DT2 (OPM Coarse Detune)
// YM2151: 0=0, 1=+approx 1.414, 2=+approx 1.58, 3=+approx 1.73
// 0: x1.0
// 1: x1.41 (600 cent up)
// 2: x1.58 (780 cent up)
// 3: x1.78 (950 cent up)
const std::array<float, 4> OpmDetune::dt2Scales = { 1.0f, 1.414f, 1.581f, 1.781f };

void OpmDetune::setParameters(int dt, int dt2)
{
    detune = dt & 7;
    detune2 = dt2 & 3;
}

float OpmDetune::noteOn(float baseFreq, int multiple) const
{
    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * dtScales[detune];

    // Multi & Detune
    float mul = (multiple == 0) ? 0.5f : (float)multiple;

    // Final Frequency = (Base + DT1) * MUL * DT2
    return detunedBaseFreq * mul * dt2Scales[detune2];
}
