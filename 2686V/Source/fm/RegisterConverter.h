#pragma once
#include <cmath>
#include <algorithm>
#include "../synth/Mode.h"

class RegisterConverter
{
public:
    static float convertFmAr(int regValue);
    static float convertFmRr(int regValue);
    static float convertFmDr(int regValue);
    static float convertFmSr(int regValue);
    static float convertFmSl(int regValue);
    static float convertFmTl(int regValue);
    static int convertFmMul(int regValue);
    static int convertFmDt(int regValue);
    static int convertFmDt2(int regValue);
    static float convertSsgVol(int regValue);
    static float convertSsgMixer(bool toneOff, bool noiseOff);
    static float convertSsgEnvPeriod(int regValue);
};
