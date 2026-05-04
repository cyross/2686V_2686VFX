#pragma once

#pragma once

#include <array>

#include "../../Core/Synth/AdsrParams.h"

struct WtParams
{
    // [[ Wavetable Parameters ]]

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    int  bitDepth = 3;

    // --- Sampling Rate ---
    // 0:96k, 1:55.5k, 2:48k, 3:44.1k, 4:22.05k, 5:16k, 6:8k
    int rateIndex = 6;

    int tableSize = 0;  // 0:32, 1:64
    int waveform = 0; // Waveform Select 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    // Custom Waveform Data (32 steps)
    std::array<float, 32> customWave32 = { 0.0f };
    // Custom Waveform Data (64 steps)
    std::array<float, 64> customWave64 = { 0.0f };
    // Custom Waveform Data (128 steps)
    std::array<float, 128> customWave128 = { 0.0f };
    // Custom Waveform Data (256 steps)
    std::array<float, 256> customWave256 = { 0.0f };
    bool modEnable = false;
    float modDepth = 0.0f;
    float modSpeed = 1.0f; // Ratio or Hz
    float level = 1.0f;

    // --- ADSR Filter ---

    // Bypass
    bool bypass = false;

    // Params
    SimpleAdsr adsr;
};
