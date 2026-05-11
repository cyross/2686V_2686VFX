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
const std::array<float, 4> OpnDetune::dtScales = { 0.0f, 0.001f, 0.0025f, 0.0045f };

void OpnDetune::setParameters(int dt, int mul)
{
    detune = std::clamp(dt, -3, 3);

    if (detune == 0 || detune == 4)
    {
        realDetune = 0.0f;
    }
    else if ( detune < 4) { // 1, 2, 3
        realDetune = -(dtScales[std::clamp(3 - detune, 0, 3)]);
    }
    else { // 5, 6, 7
        realDetune = -(dtScales[std::clamp(detune - 4, 0, 3)]);
    }

    multiple = mul;
    realMultiple = (multiple == 0) ? 0.5f : (float)multiple;
}

float OpnDetune::noteOn(float baseFreq) const
{
    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * realDetune;

    // Final Frequency = (Base + DT1) * MUL
    return detunedBaseFreq * realMultiple;
}
