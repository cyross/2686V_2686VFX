#pragma once

struct OplOpParams
{
    // --- Mask ---
    bool mask = false;

    int waveSelect = 0;

    int multiple = 1;      // MULTI: 0~15
    int keyScale = 0;
    int keyScaleLevel = 0; // KSL (Level Scaling: 0-3)

    bool amEnable = false;
    bool vibEnable = false; // VIB (LFO Pitch)
    bool egType = false;    // EG-TYP (Sustain Mode)

    // LFO Sync Delay
    int lfoSyncDelay = 0;
    int lfoFreqIndex = 0;
    float lfoFreq = 0.0f;

    // --- OPL/OPL3 LFO Settings ---
    float ams = 3.7f;  // AM Speed (Hz)
    float amd = 4.8f;  // AM Depth (dB)
    float pms = 6.4f;  // PM Speed (Hz)
    float pmd = 14.0f; // PM Depth (Cent)

    bool susEnable = false;

    bool regEnable = false; // RG-EN

    int rar = 31;  // AR Register (0-31 / OPL: 0-15)
    int rdr = 0;   // DR Register (0-31 / OPL: 0-15)
    int rsr = 0;   // SR / D2R Register (0-31)
    int rrr = 15;  // RR Register (0-15)
    int rsl = 0;   // SL Register (0-15)
    int rtl = 0;   // TL Register (0-127 / OPL: 0-63)
};
