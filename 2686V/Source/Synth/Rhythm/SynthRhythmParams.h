#pragma once

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"

struct RhythmPadParams
{
    float level = 1.0f;
    float pan = 0.5f;     // 0.0(L) - 1.0(R)
    int noteNumber = 36;  // MIDI Note Number (e.g., 36=C1)

    // ADPCM quality settings (it's more interesting to be able to set these individually)
    int qualityMode = 6;  // Default: ADPCM
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rateIndex = 7;    // Default: 16kHz

    // Whether to play as a "One Shot" or stop the sound on Note Off
    bool isOneShot = true;

    float release = 0.1f;

    float pcmOffset = 0.0f; // ms
    float pcmRatio = 1.0f;

    // Params
    AmpAdsrParams adsr;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;
};

struct RhythmParams
{
    // [[ Rhythm Parameters ]]

    // --- Rhythm (PCM) ---
    float level = 1.0f;
    // Global master volume
    float masterLevel = 1.0f;
    // Parameters for each individual pad
    std::array<RhythmPadParams, MaxRhythmPads> pads;

    // ユニゾン・ハーモニー用
    int unisonVoices = 1;        // 1 to 8
    int unisonDetuneCents = 0;   // cents
    float unisonSpread = 1.0f;   // 0.0 to 1.0 (Stereo width)
};