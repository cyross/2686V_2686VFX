#pragma once

#include <array>

#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Amp/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Step/EnvStep.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "Shared/Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "Shared/Generator/Fm/Fix/FmFixParams.h"
#include "Shared/Core/Synth/CommonParams.h"

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
    SsgSwEnv256Params ssgSwEnv256;
    PitchAdsrParams pitchAdsr;
    SsgSwPEnv256Params ssgSwPEnv256;
    Opzx7DetuneParams detune;
    LfoOpzx7Params lfo;
    FixModeParams fix;
    SsgDutyParams duty;
    SsgTriParams tri;

    // DUTY RATIO ENV (値はデューティ比の % 0〜100) / PEAK RATIO ENV (頂点 0〜1)
    bool dutyEnvEnable = false;
    StepEnvParams dutyEnv;
    bool peakEnvEnable = false;
    StepEnvParams peakEnv;
    SsgHwEnvParams env;
    SsgHwPEnvParams ssgHwPEnv;
    QualityParams quality;
    UnisonParams unison;

    // --- Waveform ---
    // 0: Pulse, 1: Triangle
    int waveform = 0;
};
