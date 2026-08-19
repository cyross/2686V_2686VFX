#pragma once

#include "../../../Effect/Envelope/Amp/OplAdsr/EnvOplAdsrParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "../../../Effect/Lfo/Opl/LfoOplParams.h"
#include "../../../Effect/Detune/Opl/DetuneOplParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../../Core/Synth/CommonParams.h"

struct Opl3OpParams
{
    OplAdsrParams m_adsrParams;
    bool ssgEnvEnable = false;
    SsgSwEnvParams ssgSwEnv;
    bool ssgEnv11Enable = false;
    SsgSwEnv11Params ssgSwEnv11;
    bool pitchEnvEnable = true;
    PitchAdsrParams pitchAdsr;
    bool ssgPEnv11Enable = true;
    SsgSwPEnv11Params ssgSwPEnv11;
    OplDetuneParams detune;
    LfoOplParams lfo;

    bool egType = false;    // EG-TYP (Sustain Mode)

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    // --- Mask ---
    bool mask = false;
};
