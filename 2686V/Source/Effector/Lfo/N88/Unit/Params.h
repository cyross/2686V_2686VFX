#pragma once

class N88LfoUnitParams
{
public:
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
};
