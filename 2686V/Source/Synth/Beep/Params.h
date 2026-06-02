#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/Params.h"
#include "../../Effect/Envelope/Pitch/Adsr/Params.h"
#include "../../Effect/Envelope/Amp/SsgSw/Params.h"

struct BeepParams
{
    // [[ BEEP Parameters ]]

    // --- Level ---
    float level = 1.0f;

    // --- Fix Freq ---
    bool fixedMode = false;
    float fixedFreq = 2000.0f;

	// -- Amp Envelope ---
    AmpAdsrParams adsr;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;

    // multiple & detune
    int multiple = 2;      // MULTI: 0~21
    float multipleRatio = 0.5f; // MULTI Ratio 0.5-27.57
    int detune = 0;
    int detune2 = 0;

    // ユニゾン・ハーモニー用
    int unisonVoices = 1;        // 1 to 8
    int unisonDetuneCents = 0;   // cents
    float unisonSpread = 1.0f;   // 0.0 to 1.0 (Stereo width)
};
