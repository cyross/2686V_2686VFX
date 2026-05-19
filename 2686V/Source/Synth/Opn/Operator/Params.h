#pragma once

#include "../../../Effect/Envelope/Amp/FmRgAdssr/Params.h"

struct OpnOpParams
{
    // --- Mask ---
    bool mask = false;

    FmRgAdssrParams m_adsrParams;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7

    int n88Ams = 0;

    // CSM/Fix Mode
    bool fixedMode = false;
    float fixedFreq = 440.0f;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;
};
