#pragma once

#include "../../../Effector/Envelope/Amp/Adsr/Params.h"
#include "../../../Effector/Envelope/Opl/Params.h"

class Opl3OpParams
{
public:
    AmpAdsrEnvParams ampEnv;
    OplEnvParams regEnv;

    float phaseOffset = 0.0f;

    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7

    int keyScale = 0;
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

    // --- Opl/OPL3 LFO Settings ---
    float ams = 3.7f;  // AM Speed (Hz)
    float amd = 4.8f;  // AM Depth (dB)
    float pms = 6.4f;  // PM Speed (Hz)
    float pmd = 14.0f; // PM Depth (Cent)

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;
};
