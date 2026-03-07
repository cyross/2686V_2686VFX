#pragma once
#include <cmath>
#include <algorithm>
#include "../synth/Mode.h"

class RegisterConverter
{
public:
    static int convertFmMul(int regValue);
    static int convertFmDt(int regValue);
    static int convertMmlDtToReg(int mmlDtValue);
    static float convertFmAr(int regValue);
    static float convertFmRr(int regValue);
    static float convertFmDr(int regValue);
    static float convertFmSl(int regValue);
    static float convertFmTl(int regValue);
    static int convertFmKs(int regValue);
    static bool convertFmMask(int regValue);

    // --- For Common / OPNA / OPN
    static float convertFmSr(int regValue);

    // --- For OPM / OPZX3 ---
    static int convertFmDt2(int regValue);

    // --- For OPL / OPL3 ---
    // --- 0-15 / 0-63 Scale ---
    static float convertOplAr(int regValue);
    static float convertOplDr(int regValue);
    static float convertOplRr(int regValue);
    static float convertOplSl(int regValue);
    static float convertOplTl(int regValue);
    static int convertOplMul(int regValue);
    static int convertOplDt(int regValue);
    static bool convertOplAm(int regValue);
    static bool convertOplVib(int regValue);
    static bool convertOplEgType(int regValue);
    static bool convertOplKsr(int regValue);
    static int convertOplKsl(int regValue);

    // --- For SSG ---
    static float convertSsgVol(int regValue);
    static float convertSsgMixer(bool toneOff, bool noiseOff);
    static float convertSsgEnvPeriod(int regValue);

    static int getValue(const juce::String& input, const juce::String& key, int maxVal);
    static bool isValidVal(int val);
};
