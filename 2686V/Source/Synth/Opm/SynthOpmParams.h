#pragma once

#include <array>

#include "./Operator/SynthOpmOpParams.h"
#include "../../Processor/Opm/ProcessorOpmValues.h"
#include "../../Effect/Lfo/Opm/LfoOpmParams.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"

struct OpmParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    LfoOpmParams glLfo;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    SsgHwEnvParams ssgHwEnv;
    SsgSwEnv11Params ssgSwEnv11g;

    int pan = 0;

    std::array<OpmOpParams, OpmPrValue::ops> op;
};
