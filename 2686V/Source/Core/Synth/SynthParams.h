#pragma once

#include <array>

#include "./Mode.h"

#include "../../Synth/Opna/Params.h"
#include "../../Synth/Opn/Params.h"
#include "../../Synth/Opl/Params.h"
#include "../../Synth/Opl3/Params.h"
#include "../../Synth/Opm/Params.h"
#include "../../Synth/Opzx7/Params.h"
#include "../../Synth/Ssg/Params.h"
#include "../../Synth/Wavetable/Params.h"
#include "../../Synth/Rhythm/Params.h"
#include "../../Synth/Adpcm/Params.h"
#include "../../Synth/Beep/Params.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPNA;

    // --- Monophonic Mode ---
    bool monoMode = false;

    OpnaParams opna;
    OpnParams opn;
    OplParams opl;
    Opl3Params opl3;
    OpmParams opm;
    Opzx7Params opzx7;
    SsgParams ssg;
    WtParams wt;
    RhythmParams rhythm;
    AdpcmParams adpcm;
    BeepParams beep;
};
