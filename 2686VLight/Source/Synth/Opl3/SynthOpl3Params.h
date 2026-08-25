#pragma once

#include <array>

#include "./Operator/SynthOpl3OpParams.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"

struct Opl3Params
{
    float level = 1.0f;

    AlgFbParams algFb;
    QualityParams quality;
    UnisonParams unison;
    SsgHwEnvParams ssgHwEnv;
    SsgSwEnv11Params ssgSwEnv11g;

    std::array<Opl3OpParams, Opl3PrValue::ops> op;
};
