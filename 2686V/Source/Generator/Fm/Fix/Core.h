#pragma once

class FixMode
{
    bool enable = false;
    float freq = 440.0f;
public:
    void setParameters(bool enable, float freq);
    float noteOn(float baseFreq);
};
