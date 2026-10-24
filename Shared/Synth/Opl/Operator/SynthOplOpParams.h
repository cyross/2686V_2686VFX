#pragma once

#include "Shared/Effect/Envelope/Step/EnvStep.h"

#include "Shared/Effect/Envelope/Amp/OplAdsr/EnvOplAdsrParams.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "Shared/Effect/Lfo/Opl/LfoOplParams.h"
#include "Shared/Effect/Detune/Opl/DetuneOplParams.h"
#include "Shared/Effect/Envelope/Amp/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw256/EnvSsgSw256Params.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Shared/Core/Synth/CommonParams.h"
#include "Shared/Generator/WtMod/GenWtAmpModulator.h"

struct OplOpParams
{
    // 押してから鳴り始めるまでの間 (秒)
    float delay = 0.0f;

    OplAdsrParams m_adsrParams;
    bool ssgEnv256Enable = false;
    SsgSwEnv256Params ssgSwEnv256;

    // TL ENV。段の TL を押したときの TL と置き換える
    bool tlEnvEnable = false;
    StepEnvParams tlEnv;

    // TL ENV の値は 0〜1 で持つ (4.3.0 から PARAM ENV の TL)。これを掛けて TL のレジスタ値にする
    float tlEnvMax = 127.0f;
    bool pitchEnvEnable = true;
    PitchAdsrParams pitchAdsr;
    bool ssgPEnv256Enable = true;
    SsgSwPEnv256Params ssgSwPEnv256;
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
