#pragma once

#include <array>

#include "../synth/AdsrParams.h"

struct BeepParams
{
    // [[ BEEP Parameters ]]

    // --- Level ---
    float level = 1.0f;

    // --- Fix Freq ---
    bool fixedMode = false;
    float fixedFreq = 2000.0f;

    // --- ADSR Filter ---

    // Bypass
    bool bypass = false;

    // Params
    SimpleAdsr adsr;
};
