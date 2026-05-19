#pragma once

#include <array>

#include "./Operator/Params.h"

struct Opl3Params
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
    // Default: 49.7kHz (Typical OPL Chip Rate)
    int fmRateIndex = 3;

    // Parameters for Operator
    std::array<Opl3OpParams, 4> op;
};
