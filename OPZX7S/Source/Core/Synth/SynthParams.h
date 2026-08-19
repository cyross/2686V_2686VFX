#pragma once

#include <array>

#include "./SynthMode.h"

#include "../../Synth/Opzx7/SynthOpzx7Params.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPZX7;

    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    Opzx7Params opzx7;
	CurveParams curve; 
};
