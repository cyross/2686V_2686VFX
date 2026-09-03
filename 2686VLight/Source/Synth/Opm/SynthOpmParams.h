#pragma once

#include <array>

#include "./Operator/SynthOpmOpParams.h"
#include "../../Processor/Opm/ProcessorOpmValues.h"
#include "../../Effect/Lfo/Opm/LfoOpmParams.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"

struct OpmParams
{
    float level = 1.0f;

    AlgFbParams algFb;
    LfoOpmParams glLfo;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    WtModParams wtMod;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;
    SsgSwEnv11Params ssgSwEnv11g;
    SsgSwPEnv11Params ssgSwPEnv11g;

    int pan = 0;

    std::array<OpmOpParams, OpmPrValue::ops> op;
};
