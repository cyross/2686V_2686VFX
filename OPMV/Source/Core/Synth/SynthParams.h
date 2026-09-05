#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opm/SynthOpmParams.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPM;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    OpmParams opm;
};
