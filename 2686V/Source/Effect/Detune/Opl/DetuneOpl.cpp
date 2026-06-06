#include <algorithm>

#include "./DetuneOpl.h"

const std::array<float, 16> OplDetune::mulScales = {
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

void OplDetune::setParameters(int mul)
{
    multiple = mul;
    realMultiple = mulScales[multiple];
}

float OplDetune::noteOn(float baseFreq) const
{
    // Final Frequency = Base * MUL
    return baseFreq * realMultiple;
}
