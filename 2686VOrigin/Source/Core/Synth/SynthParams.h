#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opna/SynthOpnaParams.h"
#include "../../Synth/Ssg/SynthSsgParams.h"
#include "../../Synth/Rhythm/SynthRhythmParams.h"
#include "../../Synth/Adpcm/SynthAdpcmParams.h"

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
    SsgParams ssg;
    RhythmParams rhythm;
    AdpcmParams adpcm;
};
