#pragma once

#include <array>

#include "Mode.h"
#include "AdsrParams.h"
#include "../../Synth/OPNA/OpnaParams.h"
#include "../../Synth/OPN/OpnParams.h"
#include "../../Synth/OPL/OplParams.h"
#include "../../Synth/OPL3/Opl3Params.h"
#include "../../Synth/OPM/OpmParams.h"
#include "../../Synth/OPZX3/Opzx3Params.h"
#include "../../Synth/SSG/SsgParams.h"
#include "../../Synth/Wavetable/WtParams.h"
#include "../../Synth/RHYTHM/RhythmParams.h"
#include "../../Synth/ADPCM/AdpcmParams.h"
#include "../../Synth/BEEP/BeepParams.h"

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
    Opzx3Params opzx3;
    SsgParams ssg;
    WtParams wt;
    RhythmParams rhythm;
    AdpcmParams adpcm;
    BeepParams beep;
};
