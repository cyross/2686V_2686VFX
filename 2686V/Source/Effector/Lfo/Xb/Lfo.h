#pragma once

// X-BASIC風LFO

#include "./Unit/Unit.h"
#include "./Params.h"
#include "../Counter/Counter.h"

class XbLfo
{
public:
	void setParameters(const XbLfoParams& params);
	void prepare(const LfoCounter* pCounter);

    XbLfoUnit am;
    XbLfoUnit pm;
    const LfoCounter* counter;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;

    // LFO Phase (0.0 - 1.0)
    double phase = 0.0f;
};
