#pragma once

#include <array>

#include "../../Effector/Envelope/Amp/Adsr/Params.h"

struct AdpcmParams
{
    // [[ ADPCM Parameters ]]

    // --- ADPCM ---
    float level = 1.0f;
    float pan = 0.5f; // ADDED: Pan
    bool loop = false; // Default false for drums
    int rootNote = 60; // Root Key (Middle C)
    int qualityMode = 6; // Default: ADPCM
    int rateIndex = 3;   // Default: 16kHz

    float offset = 0.0f; // ms
    float ratio = 1.0f;

    // --- ADSR Filter ---

    // Bypass
    bool bypass = false;

    // Params
    AmpAdsrEnvParams adsr;
};
