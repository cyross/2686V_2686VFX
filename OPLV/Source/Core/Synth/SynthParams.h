#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opl/SynthOplParams.h"
#include "../../Synth/Opl3/SynthOpl3Params.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPL;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    OplParams opl;
    Opl3Params opl3;
};
