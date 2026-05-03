#pragma once

class OpnaLfoUnitParams
{
public:
    // LFO Enable Flag
    bool enable = false;

    // Speed Freq (approx 3Hz - 30Hz)
    float freq = 5.0f;
	int freqIndex = 0;

    int wave = 0;

    float sensitivity;

    int egLfoWave = 0;

    // LFO Smooth Ratio (0.005 - 0.5)
    float smoothRate = 0.005f;
};
