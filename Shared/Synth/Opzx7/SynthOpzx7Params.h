#pragma once

#include "Shared/Effect/Envelope/Step/EnvStep.h"

#include <array>

#include "Shared/Core/Synth/ParamMod.h"

#include "./Operator/SynthOpzx7OpParams.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "Shared/Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw256/EnvSsgSw256Params.h"

struct Opzx7Params
{
    float level = 1.0f;

    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    Opzx7AlgFbParams algFb;

    // ALG ENV。段の ALG を ALG の代わりに使う
    bool algEnvEnable = false;
    StepEnvParams algEnv;
    LfoOpzx7Params glLfo;
    QualityParams quality;
    UnisonParams unison;
    PanpotParams panpot;
    AmpAdsrParams ampEnvG;
    WtModParams wtMod;
    WtAmpModParams wtAmpMod;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;
    SsgSwEnv256Params ssgSwEnv256g;
    SsgSwPEnv256Params ssgSwPEnv256g;

    std::array<Opzx7OpParams, Opzx7PrValue::ops> op;

    // PARAM ENV[256] / TAP PARAM SEQUENCER (4.3.0)。ボイスごとに値を置き換える
    ParamMod::ChipParams paramMod;
};
