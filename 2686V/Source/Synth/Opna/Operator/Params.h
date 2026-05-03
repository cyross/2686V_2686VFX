#pragma once

#include "../../../Generator/Fm/Fix/Params.h"
#include "../../../Effector/Envelope/Amp/Adssr/Params.h"
#include "../../../Effector/Envelope/Fm/Opn/Params.h"
#include "../../../Effector/Envelope/Ssg/Params.h"
#include "../../../Effector/Lfo/Global/Params.h"
#include "../../../Effector/Lfo/N88/Params.h"

class OpnaOpParams
{
public:
    AmpAdssrEnvParams ampEnv;
    OpnEnvParams regEnv;

    bool am = false;
    int ams = 0;

    // Wave Select (0:Sine, 1:Half, 2:Abs, 3:Quarter)
    int waveSelect = 0;

	SsgEnvParams ssgEnv;
    FixModeParams fixMode;

	GlobalLfoParams globalLfo;
    N88LfoParams n88lfo;
    float phaseOffset = 0.0f;

    // --- Mask ---
    bool mask = false;

    int multiple = 1;      // MULTI: 0~15
    int detune = 0;        // DT: 0~7

    int keyScale = 0;
};
