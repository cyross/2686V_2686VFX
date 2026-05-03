#pragma once

// X-BASIC風LFO

#include "./Params.h"

class XbLfoParams
{
public:
    XbLfoUnitParams am;
    XbLfoUnitParams pm;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;
};
