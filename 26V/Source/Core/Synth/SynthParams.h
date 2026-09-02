#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opn/SynthOpnParams.h"
#include "../../Synth/Ssg/SynthSsgParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPN;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    OpnParams opn;
    SsgParams ssg;
};
