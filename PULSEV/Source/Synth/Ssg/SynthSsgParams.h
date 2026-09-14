#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Generator/Fm/Fix/FmFixParams.h"
#include "../../Core/Synth/CommonParams.h"

struct SsgParams
{
    float level = 1.0f;

    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    // 再生速度。ノートによる速さの変化へ掛ける。
    float speed = 1.0f;

    // ホールドと部分再生。止まったときは MIN / MAX の値を出し続ける。
    WaveHoldParams hold;

    ToneNoiseParams tn;
    AmpAdsrParams adsr;
    WtModParams wtMod;
    WtAmpModParams wtAmpMod;
    SsgSwEnvParams ssgSwEnv;
    SsgSwEnv11Params ssgSwEnv11;
    PitchAdsrParams pitchAdsr;
    SsgSwPEnv11Params ssgSwPEnv11;
    Opzx7DetuneParams detune;
    LfoOpzx7Params lfo;
    FixModeParams fix;
    SsgDutyParams duty;
    SsgTriParams tri;
    SsgHwEnvParams env;
    SsgHwPEnvParams ssgHwPEnv;
    QualityParams quality;
    UnisonParams unison;

    // --- Waveform ---
    // 0: Pulse, 1: Triangle
    int waveform = 0;
};
