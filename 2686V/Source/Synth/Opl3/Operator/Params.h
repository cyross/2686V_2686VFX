#pragma once

#include "../../../Effect/Envelope/Amp/OplAdsr/Params.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Params.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Params.h"

struct Opl3OpParams
{
    // --- Mask ---
    bool mask = false;

    OplAdsrParams m_adsrParams;

    int multiple = 1;      // MULTI: 0~15

    bool amEnable = false;
    bool vibEnable = false; // VIB (LFO Pitch)
    bool egType = false;    // EG-TYP (Sustain Mode)

    // LFO Sync Delay
    int lfoFreqIndex = 0;
    float lfoFreq = 0.0f;

    // オペレーターごとのLFO個別感度（上乗せ分）
    float pms = 0; // 個別 Pitch Mod Sensitivity (0-7)
    float pmd = 0;
    float ams = 0; // 個別 Amp Mod Sensitivity (0-3)
    float amd = 0;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    bool pitchEnvEnable = true;
    bool ssgEnvEnable = false;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;
};
