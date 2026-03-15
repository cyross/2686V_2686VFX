#include "FmCore.h"

const std::array<FmCore::BaseAlgRouting, 8> FmCore::baseRoutings = { {
    // in2_1, in3_1, in3_2, in4_1, in4_2, in4_3, out_1, out_2, out_3, out_4
    { 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 0: 1->2->3->4
    { 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 1: (1+2)->3->4
    { 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 2: (1+3)->4, 2->3
    { 1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 3: (1->2) + 3 -> 4
    { 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 4: (1->2) + (3->4)
    { 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 5: 1 -> (2, 3, 4)
    { 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 6: (1->2) + 3 + 4
    { 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }  // 7: 1 + 2 + 3 + 4
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPNA, OPN, OPM, OPZX3 共通)
// -----------------------------------------------------------
const std::array<FmCore::LfoCalculator, 5> FmCore::lfoStrategies = { {
    // 0: Sine
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        float pm = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
        return { pm, (pm + 1.0f) * 0.5f };
    },
    // 1: Saw Down
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        return { (float)(1.0 - phase * 2.0), (float)(1.0 - phase) };
    },
    // 2: Square
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 3: Triangle
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 4: Noise
    [](double /*phase*/, float noise) -> FmCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPNA, OPN, OPM, OPZX3 共通)
// -----------------------------------------------------------
const std::array<FmCore::LfoCalculator, 4> FmCore::lfoN88Strategies = { {
    // 0: Saw Up
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        return { (float)(phase * 2.0), (float)(phase) };
    },
    // 1: Square
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> FmCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Noise
    [](double /*phase*/, float noise) -> FmCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };
