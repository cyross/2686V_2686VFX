#pragma once

// 元々からあった、グローバルLFO

#include "./Unit/Params.h"

class GlobalLfoParams
{
public:
    GlobalLfoUnitParams am;
    GlobalLfoUnitParams pm;

    // Sync Delay
    float syncDelay = 0.0f;

    // Cycle Count
    int cycles = 0;
};
