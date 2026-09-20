#pragma once

#include "Shared/Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssrParams.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "Shared/Effect/Lfo/Opna/LfoOpnaParams.h"
#include "Shared/Effect/Lfo/N88/LfoN88Params.h"
#include "Shared/Effect/Detune/Opn/DetuneOpnParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Generator/Fm/Fix/FmFixParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Generator/WtMod/GenWtAmpModulator.h"

struct OpnaOpParams
{
    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    FmRgAdssrParams m_adsrParams;
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
    SsgHwEnvParams ssgHwEnv;
    WtModParams wtMod;
    OpnDetuneParams detune;
    LfoOpnaParams hwLfo;
    LfoN88OpParams n88Lfo;
    FixModeParams fix;
    SsgEgParams se;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    // --- Mask ---
    bool mask = false;
};
