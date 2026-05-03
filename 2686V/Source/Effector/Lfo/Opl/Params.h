#pragma once

// 元々からあった、グローバルLFO

#include "./Unit/Params.h"

class OplLfoParams
{
public:
    OplLfoUnitParams am;
    OplLfoUnitParams pm;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;
};
