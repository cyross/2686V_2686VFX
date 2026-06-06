#pragma once

#include "../../../Effect/Envelope/Amp/OplAdsr/EnvOplAdsrParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"

struct OplOpParams
{
    // --- Mask ---
    bool mask = false;

    OplAdsrParams m_adsrParams;

    int waveSelect = 0;

    int multiple = 1;      // MULTI: 0~15

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

    bool pitchEnvEnable = true;
    bool ssgEnvEnable = false;

    // --- Pitch Envelope ---
    PitchAdsrParams pitchAdsr;

    // --- SSG Software Envelope ---
    SsgSwEnvParams ssgSwEnv;
};
