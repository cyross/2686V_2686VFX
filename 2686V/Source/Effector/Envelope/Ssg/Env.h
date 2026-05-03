#pragma once

#include <array>

#include "./Params.h"

using SsgWaveCalculator = float(*)(double p);

class SsgEnv
{
    int envelope = 0;
    int safeWave = 0;
    float freq = 1.0f;
    double phase = 0.0;

    static const std::array<SsgWaveCalculator, 16> ssgWaveStrategies;
public:
    void setParams(const SsgEnvParams& params);
    void noteOn();
	bool isActive() const { return envelope > 0; }
    float updatePhase(float envVal, double sampleRate);
};
