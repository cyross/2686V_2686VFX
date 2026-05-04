#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/Params.h"

struct BeepParams
{
    // [[ BEEP Parameters ]]

    // --- Level ---
    float level = 1.0f;

    // --- Fix Freq ---
    bool fixedMode = false;
    float fixedFreq = 2000.0f;

    // --- ADSR Filter ---

    // Params
    AmpAdsrParams adsr;
};
