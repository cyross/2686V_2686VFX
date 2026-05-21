#pragma once

#include "../../../Effect/Envelope/Amp/FmRgAdddr/Params.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Params.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Params.h"

struct OpmOpParams
{
    // --- Mask ---
    bool mask = false;

    FmRgAdddrParams m_adsrParams;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7
    int detune2 = 0;       // DT2: OPL3/OPM Only
    int ssgEg = 0;         // SE: 0~15 (SSG-EG)
    // FM SSG-EG Frequency (SSG-EGの周期速度)
    // 0.1Hz(ゆっくり) ～ 50Hz(高速) 程度を想定
    float fmSsgEgFreq = 1.0f;

    bool amsEnable = false;

    int pgLfoWave = 0;
    int egLfoWave = 0;

    // LFO Sync Delay
    int lfoSyncDelay = 0;
    int lfoFreqIndex = 0;
    float lfoFreq = 0.0f;

    // CSM/Fix Mode
    bool fixedMode = false;
    float fixedFreq = 440.0f;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    bool pitchEnvEnable = true;
    bool ssgEnvEnable = false;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;
};
