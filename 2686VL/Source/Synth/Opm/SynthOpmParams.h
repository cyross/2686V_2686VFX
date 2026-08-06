#pragma once

#include <array>

#include "./Operator/SynthOpmOpParams.h"
#include "../../Processor/Opm/ProcessorOpmValues.h"
#include "../../Effect/Lfo/Opm/LfoOpmParams.h"
#include "../../Core/Synth/UnisonParams.h"

struct OpmParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    LfoOpmParams glLfo;
    QualityParams quality;
    UnisonParams unison;

    int pan = 0;

    std::array<OpmOpParams, OpmPrValue::ops> op;
};
