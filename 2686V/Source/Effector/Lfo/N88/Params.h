#pragma once

// N88BASIC風LFO

#include "./Unit/Params.h"

class N88LfoParams
{
public:
    N88LfoUnitParams am;
    N88LfoUnitParams pm;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;
};
