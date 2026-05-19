#pragma once

#include "../../../Effect/Envelope/Pitch/Adsr/Params.h"

struct Opl3OpParams
{
    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    int keyScale = 0;
    int keyScaleLevel = 0; // KSL (Level Scaling: 0-3)

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

    bool regEnable = false; // RG-EN

    int rar = 31;  // AR Register (0-31 / OPL: 0-15)
    int rdr = 0;   // DR Register (0-31 / OPL: 0-15)
    int rsr = 0;   // SR / D2R Register (0-31)
    int rrr = 15;  // RR Register (0-15)
    int rsl = 0;   // SL Register (0-15)
    int rtl = 0;   // TL Register (0-127 / OPL: 0-63)
};
