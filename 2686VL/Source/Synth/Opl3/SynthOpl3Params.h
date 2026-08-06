#pragma once

#include <array>

#include "./Operator/SynthOpl3OpParams.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Core/Synth/UnisonParams.h"

struct Opl3Params
{
    float level = 1.0f;

    AlgFbParams algFb;
    QualityParams quality;
    UnisonParams unison;

    std::array<Opl3OpParams, Opl3PrValue::ops> op;
};
