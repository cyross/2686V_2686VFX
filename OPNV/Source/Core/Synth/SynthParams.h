#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opna/SynthOpnaParams.h"
#include "../../Synth/Opn/SynthOpnParams.h"
#include "Shared/Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPNA;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    OpnaParams opna;
    OpnParams opn;
};
