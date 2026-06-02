#include <algorithm>

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
const std::array<float, 8> OpnDetune::dtScales = { 0.0f, -0.0045f, -0.0025f, -0.001f, 0.0f, 0.001f, 0.0025f, 0.0045f };

const std::array<float, 16> OpnDetune::mulScales = {
    0.5f,   // 0: x0.5
    1.0f,   // 1: x1.0
    2.0f,   // 2: x2.0
    3.0f,   // 3: x3.0
    4.0f,   // 4: x4.0
    5.0f,   // 5: x5.0
    6.0f,   // 6: x6.0
    7.0f,   // 7: x7.0
    8.0f,   // 8: x8.0
    9.0f,   // 9: x9.0
    10.0f,  // 10: x10.0
    11.0f,  // 11: x11.0
    12.0f,  // 12: x12.0
    13.0f,  // 13: x13.0
    14.0f,  // 14: x14.0
    15.0f   // 15: x15.0
};

void OpnDetune::setParameters(int dt, int mul)
{
    detune = std::clamp(dt, -3, 3);
    realDetune = dtScales[detune + 3];

    multiple = mul;
    realMultiple = mulScales[multiple];
}

float OpnDetune::noteOn(float baseFreq) const
{
    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * realDetune;

    // Final Frequency = (Base + DT1) * MUL
    return detunedBaseFreq * realMultiple;
}
