#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opna/SynthOpnaParams.h"
#include "../../Synth/Opn/SynthOpnParams.h"
#include "../../Synth/Opl/SynthOplParams.h"
#include "../../Synth/Opl3/SynthOpl3Params.h"
#include "../../Synth/Opm/SynthOpmParams.h"
#include "../../Synth/Opzx7/SynthOpzx7Params.h"
#include "../../Synth/Ssg/SynthSsgParams.h"
#include "../../Synth/Wavetable/SynthWtParams.h"
#include "../../Synth/Rhythm/SynthRhythmParams.h"
#include "../../Synth/Adpcm/SynthAdpcmParams.h"
#include "../../Synth/Beep/SynthBeepParams.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

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
    OplParams opl;
    Opl3Params opl3;
    OpmParams opm;
    Opzx7Params opzx7;
    SsgParams ssg;
    WtParams wt;
    RhythmParams rhythm;
    AdpcmParams adpcm;
    BeepParams beep;
	CurveParams curve; 
};
