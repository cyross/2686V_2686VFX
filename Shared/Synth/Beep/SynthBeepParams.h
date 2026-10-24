#pragma once

#include <array>

#include "Shared/Core/Synth/ParamMod.h"

#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Amp/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "Shared/Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "Shared/Generator/Fm/Fix/FmFixParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"

struct BeepParams
{
    float level = 1.0f;

    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    // 帯域制限 (PolyBLEP) でエイリアスノイズを抑えるかどうか
    bool antiAlias = false;

    // タイマの基準クロック選択 (1 = Free)
    int timerClock = 1;

    AmpAdsrParams adsr;
    WtModParams wtMod;
    WtAmpModParams wtAmpMod;
    SsgSwEnv256Params ssgSwEnv256;
    PitchAdsrParams pitchAdsr;
    SsgSwPEnv256Params ssgSwPEnv256;
    Opzx7DetuneParams detune;
    LfoOpzx7Params lfo;
    FixModeParams fix;
    UnisonParams unison;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;

    // PARAM ENV[256] / TAP PARAM SEQUENCER (4.3.0)。ボイスごとに値を置き換える
    ParamMod::ChipParams paramMod;
};
