#pragma once

// 元々からあった、グローバルLFO

#include "./Unit/Params.h"

class OpmLfoParams
{
public:
    OpmLfoUnitParams am;
    OpmLfoUnitParams pm;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;
};
