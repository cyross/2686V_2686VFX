#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/Params.h"
#include "../../Effect/Envelope/Pitch/Adsr/Params.h"
#include "../../Effect/Envelope/Amp/SsgSw/Params.h"

struct AdpcmParams
{
    // [[ ADPCM Parameters ]]

    // --- ADPCM ---
    float level = 1.0f;
    float pan = 0.5f; // ADDED: Pan
    bool loop = false; // Default false for drums
    int rootNote = 60; // Root Key (Middle C)
    int qualityMode = 6; // Default: ADPCM
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rateIndex = 7;   // Default: 16kHz

    float offset = 0.0f; // ms
    float ratio = 1.0f;

	// AmpEnvelope Params
    AmpAdsrParams adsr;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;

    // detune
    int detune = 0;
    int detune2 = 0;
};
