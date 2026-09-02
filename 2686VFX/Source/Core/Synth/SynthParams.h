#pragma once

#include <array>

#include "./SynthMode.h"


struct SynthParams
{
    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

};
