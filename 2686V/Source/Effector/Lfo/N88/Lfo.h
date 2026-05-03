#pragma once

// N88BASIC風LFO

#include "./Unit/Unit.h"
#include "./Params.h"
#include "../Counter/Counter.h"

class N88Lfo
{
public:
	void setParameters(const N88LfoParams& params);
	void prepare(const LfoCounter* pCounter);

    N88LfoUnit am;
    N88LfoUnit pm;
    const LfoCounter* counter;

    // Sync Delay
    float syncDelay = 0.0f;
    float delayCounter = 0.0f;

    // Cycle Count
    int cycles = 0;
    uint32_t lfoCycleCount = 0;

    // LFO Phase (0.0 - 1.0)
    double phase = 0.0f;
};
