#pragma once

#include "./Params.h"

class XbLfoUnit
{
public:
    void setParameters(const XbLfoUnitParams& params);
    void prepare();

    // LFO Enable Flag
    bool enable = false;

    // Speed Freq (approx 3Hz - 30Hz)
    float freq = 5.0f;
	int freqIndex = 0;

    int wave = 0;

    int sensitivity;
    int depth;

    int egLfoWave = 0;

    // LFO Smooth Ratio (0.005 - 0.5)
    float smoothRate = 0.005f;

    double timerAcc = 1.0;
    float steppedVal = 0.0f;
    float smooth = 0.0f;
    float value = 0.0f;
};
