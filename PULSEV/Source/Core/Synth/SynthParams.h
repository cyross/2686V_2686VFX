#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Ssg/SynthSsgParams.h"
#include "../../Synth/Beep/SynthBeepParams.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::SSG;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    SsgParams ssg;
    BeepParams beep;
};
