#pragma once

#include "../../../Effect/Envelope/Pitch/Adsr/Params.h"

struct OpmOpParams
{
    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    float mutipleRatio = 0.5f; // MULTI Ratio 0.5-27.57
    int detune = 0;        // DT: 0~7
    int detune2 = 0;       // DT2: OPL3/OPM Only
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
    int lfoSyncDelay = 0;
    int lfoFreqIndex = 0;
    float lfoFreq = 0.0f;

    // オペレーターごとのLFO個別感度（上乗せ分）
    float pms = 0; // 個別 Pitch Mod Sensitivity (0-7)
    float pmd = 0;
    float ams = 0; // 個別 Amp Mod Sensitivity (0-3)
    float amd = 0;

    // CSM/Fix Mode
    bool fixedMode = false;
    float fixedFreq = 440.0f;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

    bool regEnable = false; // RG-EN

    int rar = 31;  // AR Register (0-31 / OPL: 0-15)
    int rdr = 0;   // DR Register (0-31 / OPL: 0-15)
    int rsr = 0;   // SR / D2R Register (0-31)
    int rrr = 15;  // RR Register (0-15)
    int rsl = 0;   // SL Register (0-15)
    int rtl = 0;   // TL Register (0-127 / OPL: 0-63)

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;
};
