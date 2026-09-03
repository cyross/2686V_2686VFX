#pragma once

#include "../../../Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssrParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "../../../Effect/Lfo/Opna/LfoOpnaParams.h"
#include "../../../Effect/Lfo/N88/LfoN88Params.h"
#include "../../../Effect/Detune/Opn/DetuneOpnParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"
#include "../../../Generator/Fm/Fix/FmFixParams.h"
#include "../../../Core/Synth/CommonParams.h"
#include "../../../Generator/WtMod/GenWtAmpModulator.h"

struct OpnaOpParams
{
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
