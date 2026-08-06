#pragma once

#include <array>

#include "./Operator/SynthOplOpParams.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Core/Synth/UnisonParams.h"

struct OplParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    QualityParams quality;
    UnisonParams unison;

    std::array<OplOpParams, OplPrValue::ops> op;
};
