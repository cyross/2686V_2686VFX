#include <cmath>
#include <algorithm>

#include "./DetuneOpzx7.h"

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
const std::array<float, 16> Opzx7Detune::dtScales = {
    0.0f, -0.008119f, -0.00696f, -0.00579f, -0.00463f, -0.00347f, -0.00231f, -0.00116f,
    0.0f, 0.00116f, 0.00231f, 0.00347f, 0.00463f, 0.00579f, 0.00696f, 0.008119f
};

// DT2 (OPM Coarse Detune)
// YM2151: 0=0, 1=+approx 1.414, 2=+approx 1.58, 3=+approx 1.73
// 0: x1.0
// 1: x1.41 (600 cent up)
// 2: x1.58 (780 cent up)
// 3: x1.78 (950 cent up)
const std::array<float, 4> Opzx7Detune::dt2Scales = { 1.0f, 1.414f, 1.581f, 1.781f };

Opzx7Detune::Opzx7Detune()
{
	mulScales = {
		0.5f,   // 0:  x 0.5
		0.891f, // 1:  x 0.891 (approx sqrt(0.8))
		1.0f,   // 2:  x 1.0
		1.414f, // 3:  x 1.414 (approx sqrt(2))
		1.498f, // 4:  x 1.498 (approx sqrt(2.25))
		1.581f, // 5:  x 1.581 (approx sqrt(2.5))
		1.781f, // 6:  x 1.781 (approx sqrt(3.17))
		2.0f,   // 7:  x 2.0
		3.0f,   // 8:  x 3.0
		4.0f,   // 9:  x 4.0
		5.0f,   // 10: x 5.0
		6.0f,   // 11: x 6.0
		7.0f,   // 12: x 7.0
		8.0f,   // 13: x 8.0
		9.0f,   // 14: x 9.0
		10.0f,  // 15: x 10.0
		11.0f,  // 16: x 11.0
		12.0f,  // 17: x 12.0
		13.0f,  // 18: x 13.0
		14.0f,  // 19: x 14.0
		15.0f,  // 20: x 15.0
		0.0f    // 21: x mulRatio
	};

	for (int i = 0; i < 10000; i++) {
		dt3Scales[i] = std::pow(2.0, (float)(i - 4800) / 1200.0);
	}

	detune = 0;
	realDetune = dtScales[detune];

	detune2 = 0;
	realDetune2 = dt2Scales[detune2];

	multiple = 2; // デフォルトは x1.0
	realMultiple = mulScales[multiple];

	detune3 = 0;
	realDetune3 = 0.0f;
}

void Opzx7Detune::setParameters(int dt, int dt2, int dt3, int mul, float mulRatio)
{
    detune = dt & 15;
    realDetune = dtScales[detune];

    detune2 = dt2 & 3;
    realDetune2 = dt2Scales[detune2];

	detune3 = dt3;
	realDetune3 = dt3Scales[std::clamp(detune3, -4800, 4800) + 4800]; // detune3 = -4800 - 4800

    multiple = mul;
	mulScales[21] = mulRatio; // 21番目にユーザー指定のmulRatioをセット
	realMultiple = mulScales[multiple];
}

float Opzx7Detune::noteOn(float baseFreq) const
{
    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * realDetune;

    // Final Frequency = (Base + DT1) * MUL * DT2 * DT3
    return detunedBaseFreq * realMultiple * realDetune2 * realDetune3;
}
