#pragma once

#include <array>

#include "./Operator/SynthOpnOpParams.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Effect/Lfo/N88/LfoN88Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"

struct OpnParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    LfoN88Params glLfo;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    SsgHwEnvParams ssgHwEnv;
    SsgSwEnv11Params ssgSwEnv11g;
    SsgSwPEnv11Params ssgSwPEnv11g;

    std::array<OpnOpParams, OpnPrValue::ops> op;
};
