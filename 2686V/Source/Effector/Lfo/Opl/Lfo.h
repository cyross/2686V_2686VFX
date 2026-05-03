#pragma once

// 元々からあった、グローバルLFO

#include "./Unit/Unit.h"
#include "./Params.h"
#include "../Counter/Counter.h"

class OplLfo
{
public:
	void setParameters(const OplLfoParams& params);
	void prepare(const LfoCounter* pCounter);

    OplLfoUnit am;
    OplLfoUnit pm;
    const LfoCounter* counter;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;

    // LFO Phase (0.0 - 1.0)
    double phase = 0.0;
};
