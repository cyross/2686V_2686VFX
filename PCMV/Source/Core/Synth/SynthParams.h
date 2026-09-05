#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Rhythm/SynthRhythmParams.h"
#include "../../Synth/Adpcm/SynthAdpcmParams.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::RHYTHM;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    RhythmParams rhythm;
    AdpcmParams adpcm;
};
