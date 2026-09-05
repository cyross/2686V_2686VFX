#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Wavetable/SynthWtParams.h"
#include "../../Synth/Wt2/SynthWt2Params.h"
#include "../../Synth/WtPlus/SynthWtPlusParams.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::WAVETABLE;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    WtParams wt;
    Wt2Params wt2;
    WtPlusParams wtPlus;
};
