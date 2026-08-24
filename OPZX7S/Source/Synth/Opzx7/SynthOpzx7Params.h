#pragma once

#include <array>

#include "./Operator/SynthOpzx7OpParams.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"

struct Opzx7Params
{
    float level = 1.0f;

    Opzx7AlgFbParams algFb;
    LfoOpzx7Params glLfo;
    QualityParams quality;
    UnisonParams unison;
    PanpotParams panpot;

    std::array<Opzx7OpParams, Opzx7PrValue::ops> op;
};
