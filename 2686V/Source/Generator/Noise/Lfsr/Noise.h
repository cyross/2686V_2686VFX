#pragma once

class LfsrNoiseGen
{
public:
    void setup();
    float generateSample();
    void updateNoiseDelta(double targetRate);

    unsigned int lfsr = 0x1FFFF;

    float phase = 0.0f;
    float delta = 0.0f;
    float currentSample = 0.0f;
    float targetFreq = 12000.0f;
};
