#pragma once

#include <array>

#include "./Operator/SynthOpzx7OpParams.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"

struct Opzx7Params
{
    float level = 1.0f;

    Opzx7AlgFbParams algFb;
    LfoOpzx7Params glLfo;
    QualityParams quality;
    UnisonParams unison;
    PanpotParams panpot;
    AmpAdsrParams ampEnvG;
    WtModParams wtMod;
    SsgHwEnvParams ssgHwEnv;
    SsgSwEnv11Params ssgSwEnv11g;
    SsgSwPEnv11Params ssgSwPEnv11g;

    std::array<Opzx7OpParams, Opzx7PrValue::ops> op;
};
