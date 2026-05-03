#pragma once

#include "../../../Generator/Fm/Fix/FixModeParams.h"
#include "../../../Envelope/Amp/Adssr/Params.h"
#include "../../../Envelope/Opn/Params.h"

class OpnOpParams
{
public:
    AmpAdssrEnvParams ampEnv;
    OpnEnvParams regEnv;

    float phaseOffset = 0.0f;

    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7

    int keyScale = 0;
    float sustainRate = 0.0f; // SR: Sustain Rate

    int detune2 = 0;       // DT2: Opl3/OPM Only
    int keyScale = 0;
    int keyScaleLevel = 0; // KSL (Level Scaling: 0-3)
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

    int rsr = 0;   // SR / D2R Register (0-31)
};
