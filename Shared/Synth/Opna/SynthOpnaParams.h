#pragma once

#include "Shared/Effect/Envelope/Step/EnvStep.h"

#include <array>

#include "Shared/Core/Synth/ParamMod.h"

#include "./SynthOpnaParams.h"
#include "./Operator/SynthOpnaOpParams.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "Shared/Effect/Lfo/N88/LfoN88Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw256/EnvSsgSw256Params.h"

struct OpnaParams
{
    float level = 1.0f;

    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    AlgFbParams algFb;

    // ALG ENV。段の ALG を ALG の代わりに使う
    bool algEnvEnable = false;
    StepEnvParams algEnv;
    LfoN88Params glLfo;
    QualityParams quality;
    UnisonParams unison;
    AmpAdsrParams ampEnvG;
    WtModParams wtMod;
    WtAmpModParams wtAmpMod;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;
    SsgSwEnv256Params ssgSwEnv256g;
    SsgSwPEnv256Params ssgSwPEnv256g;

    int pan = 0;

    std::array<OpnaOpParams, OpnaPrValue::ops> op;

    // PARAM ENV[256] / TAP PARAM SEQUENCER (4.3.0)。ボイスごとに値を置き換える
    ParamMod::ChipParams paramMod;
};
