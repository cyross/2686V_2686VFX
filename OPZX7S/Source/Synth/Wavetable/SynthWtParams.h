#pragma once

#include <array>

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11Params.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7Params.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7Params.h"
#include "../../Core/Synth/UnisonParams.h"
#include "../../Generator/Fm/Fix/FmFixParams.h"
#include "../../Core/Synth/CommonParams.h"

struct WtParams
{
    float level = 1.0f;

    AmpAdsrParams adsr;
    SsgSwEnvParams ssgSwEnv;
    SsgSwEnv11Params ssgSwEnv11;
    PitchAdsrParams pitchAdsr;
    SsgSwPEnv11Params ssgSwPEnv11;
    Opzx7DetuneParams detune;
    LfoOpzx7Params lfo;
    FixModeParams fix;
    WtModParams mod;
    QualityParams quality;
    UnisonParams unison;

    int tableSize = 0; // 0:32, 1:64
    int steps = 0;     // Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
    int waveform = 8;  // Waveform Select 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    // Custom Waveform Data (32 steps)
    std::array<float, 32> customWave32 = { 0.0f };
    // Custom Waveform Data (64 steps)
    std::array<float, 64> customWave64 = { 0.0f };
    // Custom Waveform Data (128 steps)
    std::array<float, 128> customWave128 = { 0.0f };
    // Custom Waveform Data (256 steps)
    std::array<float, 256> customWave256 = { 0.0f };
};
