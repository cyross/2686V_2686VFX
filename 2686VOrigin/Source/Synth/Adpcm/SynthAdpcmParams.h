#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Generator/Fm/Fix/FmFixParams.h"
#include "../../Core/Synth/CommonParams.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"

struct AdpcmParams
{
    float level = 1.0f;

    ToneNoiseParams tn;
    AmpAdsrParams adsr;
    WtModParams wtMod;
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
    UnisonParams unison;
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;

    float pan = 0.5f; // ADDED: Pan
    bool loop = false; // Default false for drums
    int rootNote = 60; // Root Key (Middle C)
};
