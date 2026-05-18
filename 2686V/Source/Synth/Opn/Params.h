#pragma once

#include <array>

#include "../../Core/Fm/FmOpParams.h"

struct OpnParams
{
    // [[ FM Parameters ]]

    // --- Algorithm ---
    int algorithm = 7;

    // --- Feedback ---
    float feedback = 0.0f;

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    // Default: Raw (32bit float) or 3 (8bit) depending on preference
    int fmBitDepth = 4;

    // --- Sampling Rate ---
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    // Default: 55.5kHz (Typical FM Chip Rate)
    int fmRateIndex = 2;

    // -- OPNA/OPM/OPZX7 LFO(Global) ---

    // LFO Speed Freq (approx 3Hz - 30Hz)
    float lfoFreq = 5.0f;

    // LFO Enable Flag
    bool amEnable = false;
    bool pmEnable = false;

    // LFO Sensitivity

    // Pitch Modulation Sensitivity (0-7/0.0-1.0)
    float lfoPms = 0.0f;

    // Amplitude Modulation Sensitivity (0-3/0.0-1.0)
    float lfoAms = 0.0f;

    // Pitch Modulation Depth (0-127/0.0-1.0)
    float lfoPmd = 0.0f;

    // Amplitude Modulation Depth (0-127/0.0-1.0)
    float lfoAmd = 0.0f;

    // LFO Waveform (0:Saw, 1:Square, 2:Triangle, 3:Random)
    int lfoWave = 2;
    int pgLfoWave = 0;
    int egLfoWave = 0;

    // LFO AM Smooth Ratio (0.005 - 0.5)
    float lfoAmSmRt = 0.005f;

    // LFO Sync Delay
    int lfoSyncDelay = 0;

    // --- OPLL Preset ---
    int opllPreset = 0; // OPLL Preset Instrument Index

    // Parameters for Operator
    std::array<FmOpParams, 4> op;
};
