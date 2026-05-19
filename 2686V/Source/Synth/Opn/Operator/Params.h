#pragma once

#include "../../../Effect/Envelope/Pitch/Adsr/Params.h"

struct OpnOpParams
{
    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7
    int keyScale = 0;

    int n88Ams = 0;

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
};
