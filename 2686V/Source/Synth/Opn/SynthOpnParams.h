#pragma once

#include <array>

#include "./Operator/SynthOpnOpParams.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Effect/Lfo/N88/LfoN88Params.h"
#include "../../Core/Synth/UnisonParams.h"

struct OpnParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    LfoN88Params glLfo;
    QualityParams quality;
    UnisonParams unison;

    std::array<OpnOpParams, OpnPrValue::ops> op;
};
