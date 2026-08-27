#pragma once

#include <array>

#include "./SynthOpnaParams.h"
#include "./Operator/SynthOpnaOpParams.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "../../Effect/Lfo/N88/LfoN88Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"

struct OpnaParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    LfoN88Params glLfo;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    SsgHwEnvParams ssgHwEnv;
    SsgSwEnv11Params ssgSwEnv11g;

    int pan = 0;

    std::array<OpnaOpParams, OpnaPrValue::ops> op;
};
