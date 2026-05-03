#pragma once

#include "../../../Generator/Fm/Fix/Params.h"
#include "../../../Effector/Envelope/Amp/Adddr/Params.h"
#include "../../../Effector/Envelope/Fm/Opm/Params.h"

class Opzx3OpParams
{
public:
    AmpAdddrEnvParams ampEnv;
    OpmEnvParams regEnv;

    float phaseOffset = 0.0f;

    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7

    int keyScale = 0;
    int detune2 = 0;       // DT2: Opl3/OPM Only
    int ssgEg = 0;         // SE: 0~15 (SSG-EG)
    // FM SSG-EG Frequency (SSG-EGの周期速度)
    // 0.1Hz(ゆっくり) ～ 50Hz(高速) 程度を想定
    float fmSsgEgFreq = 1.0f;

    bool amEnable = false;
    bool vibEnable = false; // VIB (LFO Pitch)
    bool egType = false;    // EG-TYP (Sustain Mode)

    int pgLfoWave = 0;
    int egLfoWave = 0;

    // LFO Sync Delay
    float lfoSyncDelay = 0.0f;
    int lfoFreqIndex = 0;
    float lfoFreq = 0.0f;

    // オペレーターごとのLFO個別感度（上乗せ分）
    float pms = 0; // 個別 Pitch Mod Sensitivity (0-7)
    float pmd = 0;
    float ams = 0; // 個別 Amp Mod Sensitivity (0-3)
    float amd = 0;

    FixModeParams fixMode;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    float pcmOffset = 0.0f; // ms
    float pcmRatio = 1.0f;

    float phaseOffset = 0.0f; // OPZX3-PHASE-OFFSET
};
