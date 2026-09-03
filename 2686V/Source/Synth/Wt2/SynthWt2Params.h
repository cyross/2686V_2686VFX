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
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHwParams.h"

struct Wt2Params
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
    SsgHwEnvParams ssgHwEnv;
    SsgHwPEnvParams ssgHwPEnv;

    // 波形テーブルの読み出しを線形補間するか。
    // OFF なら実機の波形メモリと同じ階段状の出力になる。
    bool interpolate = true;

    int tableSize = 0; // 0:32, 1:64
    int waveform = 8;  // Waveform Select 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom

    // --- カスタム波形の解像度 ---
    // 0: 4-bit (0-15, センター8)
    // 1: 5-bit (0-31, センター16)
    // 2: 6-bit (0-63, センター32)
    // 3: 7-bit (0-127, センター64)
    // 4: 8-bit (0-255, センター128)
    int customWaveResolution = 0;

    // Custom Waveform Data (32 steps)
    std::array<int, 32> customWave32 = { 0 };
    // Custom Waveform Data (64 steps)
    std::array<int, 64> customWave64 = { 0 };
    // Custom Waveform Data (128 steps)
    std::array<int, 128> customWave128 = { 0 };
    // Custom Waveform Data (256 steps)
    std::array<int, 256> customWave256 = { 0 };

    Wt2Params() {
        customWave32.fill(8);
        customWave64.fill(8);
        customWave128.fill(8);
        customWave256.fill(8);
    }
};
