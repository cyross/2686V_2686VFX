#pragma once

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Generator/Fm/Fix/FmFixParams.h"
#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"

struct WtPlusParams
{
    float level = 1.0f;

    AmpAdsrParams adsr;
    SsgSwEnvParams ssgSwEnv;
    SsgSwEnv11Params ssgSwEnv11;
    PitchAdsrParams pitchAdsr;
    SsgSwPEnv11Params ssgSwPEnv11;
    Opzx7DetuneParams detune;
    LfoOpzx7Params lfo;
    FixModeParams fix;
    WtModParams mod;
    QualityParams quality;
    UnisonParams unison;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;

    // 鳴らす波形メモリのスロット番号 (0 〜 Global::WtPlus::slots - 1)。
    // オートメーションで振れるので、切り替えは即時に反映する。
    int slot = 0;

    // 波形テーブルの読み出しを線形補間するか。
    // OFF なら実機の波形メモリと同じ階段状の出力になる。
    bool interpolate = true;

    // Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
    int steps = 0;
};
