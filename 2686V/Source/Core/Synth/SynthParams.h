#pragma once

#include <array>

#include "Mode.h"
#include "AdsrParams.h"
#include "../../OPNA/OpnaParams.h"
#include "../../OPN/OpnParams.h"
#include "../../OPL/OplParams.h"
#include "../../OPL3/Opl3Params.h"
#include "../../OPM/OpmParams.h"
#include "../../OPZX3/Opzx3Params.h"
#include "../../SSG/SsgParams.h"
#include "../../WT/WtParams.h"
#include "../../RHYTHM/RhythmParams.h"
#include "../../ADPCM/AdpcmParams.h"
#include "../../BEEP/BeepParams.h"

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
