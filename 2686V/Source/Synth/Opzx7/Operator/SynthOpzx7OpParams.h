#pragma once

#include "../../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"

struct Opzx7OpParams
{
    // --- Mask ---
    bool mask = false;

    Opzx7AdddrParams m_adsrParams;

    int multiple = 1;      // MULTI: 0~15
    float multipleRatio = 0.5f; // MULTI Ratio 0.5-27.57
    int detune = 0;        // DT: 0~7
    int detune2 = 0;       // DT2: OPL3/OPM Only
    int detune3 = 0;
    int ssgEg = 0;         // SE: 0~15 (SSG-EG)
    // FM SSG-EG Frequency (SSG-EGの周期速度)
    // 0.1Hz(ゆっくり) ～ 50Hz(高速) 程度を想定
    float fmSsgEgFreq = 1.0f;

    bool amEnable = false;
    bool vibEnable = false; // VIB (LFO Pitch)

    int pgLfoWave = 0;
    int egLfoWave = 0;

    // LFO Sync Delay
    int lfoPmSyncDelay = 0;
    int lfoAmSyncDelay = 0;
    int lfoPmFreqIndex = 0;
    int lfoAmFreqIndex = 0;
    float lfoPmFreq = 0.0f;
    float lfoAmFreq = 0.0f;
    float lfoAmSmRt = 0.0f;

    // オペレーターごとのLFO個別感度（上乗せ分）
    float pms = 0; // 個別 Pitch Mod Sensitivity (0-7)
    float pmd = 0;
    float ams = 0; // 個別 Amp Mod Sensitivity (0-3)
    float amd = 0;

    int n88Ams = 0;

    // CSM/Fix Mode
    bool fixedMode = false;
    float fixedFreq = 440.0f;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    float pcmOffset = 0.0f; // ms
    float pcmRatio = 1.0f;
    bool loopPointEnable = false;
    float loopPointStart = 0.0f; // 0.0 to 1.0
    float loopPointEnd = 1.0f;   // 0.0 to 1.0

    bool pitchEnvEnable = true;
    bool ssgEnvEnable = false;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;
};
