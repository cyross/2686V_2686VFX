#pragma once

#include <array>

#include "./Operator/SynthOpl3OpParams.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"

struct Opl3Params
{
    float level = 1.0f;

    AlgFbParams algFb;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    WtModParams wtMod;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;
    SsgSwEnv11Params ssgSwEnv11g;
    SsgSwPEnv11Params ssgSwPEnv11g;

    std::array<Opl3OpParams, Opl3PrValue::ops> op;
};
