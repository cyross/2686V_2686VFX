#pragma once

#include <array>

#include "./Operator/SynthOplOpParams.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"

struct OplParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    SsgHwEnvParams ssgHwEnv;
    SsgSwEnv11Params ssgSwEnv11g;

    std::array<OplOpParams, OplPrValue::ops> op;
};
