#pragma once

struct RhythmPadParams
{
    float level = 1.0f;
    float pan = 0.5f;     // 0.0(L) - 1.0(R)
    int noteNumber = 36;  // MIDI Note Number (e.g., 36=C1)

    // ADPCM quality settings (it's more interesting to be able to set these individually)
    int qualityMode = 6;  // Default: ADPCM
    int rateIndex = 3;    // Default: 16kHz

    // Whether to play as a "One Shot" or stop the sound on Note Off
    bool isOneShot = true;

    float release = 0.1f;

    float pcmOffset = 0.0f; // ms
    float pcmRatio = 1.0f;
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
};