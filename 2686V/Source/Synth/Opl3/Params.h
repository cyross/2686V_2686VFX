#pragma once

#include <array>

#include "../../Lfo/Global/GlobalLfoParams.h"
#include "./Operator/Params.h"

class Opl3Params
{
public:
    GlobalLfoParams lfo;

    // Parameters for Operator
    std::array<Opl3OpParams, 4> op;
    // --- Algorithm ---
    int algorithm = 7;

    // --- Feedback ---
    int feedback0 = 0;
    int feedback2 = 0;

    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    // Default: Raw (32bit float) or 3 (8bit) depending on preference
    int bitDepth = 4;

    // --- Sampling Rate ---
    // 0:96k, 1:55.5k, 2:48k, 3:44.1k, 4:22.05k, 5:16k, 6:8k
    // Default: 55.5kHz (Typical FM Chip Rate)
    int rateIndex = 1;
};
