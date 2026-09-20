#pragma once

#include "Shared/Effect/Envelope/Amp/OplAdsr/EnvOplAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "Shared/Effect/Lfo/Opl/LfoOplParams.h"
#include "Shared/Effect/Detune/Opl/DetuneOplParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Generator/WtMod/GenWtAmpModulator.h"

struct OplOpParams
{
    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    OplAdsrParams m_adsrParams;
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
    OplDetuneParams detune;
    LfoOplParams lfo;

    int waveSelect = 0;
    bool egType = false;    // EG-TYP (Sustain Mode)
    bool mask = false;
};
