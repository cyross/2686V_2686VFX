#pragma once

#include "../../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "../../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../../Generator/Fm/Fix/FmFixParams.h"
#include "../../../Core/Synth/CommonParams.h"
#include "../../../Generator/WtMod/GenWtAmpModulator.h"

struct Opzx7OpParams
{
    Opzx7AdddrParams m_adsrParams;
    bool ssgEnvEnable = false;
    SsgSwEnvParams ssgSwEnv;
    bool ssgEnv11Enable = false;
    SsgSwEnv11Params ssgSwEnv11;
    bool pitchEnvEnable = true;
    PitchAdsrParams pitchAdsr;
    bool ssgPEnv11Enable = true;
    SsgSwPEnv11Params ssgSwPEnv11;
    SsgHwPEnvParams ssgHwPEnv;
    WtAmpModParams wtAmpMod;
    Opzx7DetuneParams detune;
    LfoOpzx7Params lfo;
    FixModeParams fix;
    SsgEgParams se;
    PcmParams pcm;
    LoopPointParams lp;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    // --- Mask ---
    bool mask = false;
};
