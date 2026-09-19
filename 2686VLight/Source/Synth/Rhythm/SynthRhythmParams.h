#pragma once
#include "Shared/Generator/Pcm/Helper/GenPcmShared.h"

#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "Shared/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "Shared/Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "Shared/Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "Shared/Generator/Fm/Fix/FmFixParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"

struct RhythmPadParams
{
    float level = 1.0f;

    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

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
    PcmParams pcm;
    LoopPointParams lp;
    QualityPcmParams quality;

    // 符号化した素材。プロセッサがパッドごとに 1 つだけ持ち、ここは指すだけ。
    const PcmSharedData* source = nullptr;

    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;

    float pan = 0.5f;     // 0.0(L) - 1.0(R)
    int noteNumber = 36;  // MIDI Note Number (e.g., 36=C1)
    bool isOneShot = true;
};

struct RhythmParams
{
    float level = 1.0f;

    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    UnisonParams unison;

    std::array<RhythmPadParams, RhythmPrValue::pads> pads;
};