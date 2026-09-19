#pragma once

// ============================================================================
// 音源の本体へ渡すパラメータ
// ============================================================================
// 全音源のパラメータと、発音の仕方 (モノ・ベロシティなど) を束ねる。
// 12 本で共有する。
//
// どの音源を鳴らすか (mode) はここに置かない。OscMode の番号はプラグイン
// ごとに違い、MODE パラメータとして保存されているため。mode は各プラグインの
// SynthParams (これを受け継ぐ) が持つ。音源の本体は mode を見ない。

#include <array>

#include "../../Synth/Opna/SynthOpnaParams.h"
#include "../../Synth/Opn/SynthOpnParams.h"
#include "../../Synth/Opl/SynthOplParams.h"
#include "../../Synth/Opl3/SynthOpl3Params.h"
#include "../../Synth/Opm/SynthOpmParams.h"
#include "../../Synth/Opzx7/SynthOpzx7Params.h"
#include "../../Synth/Ssg/SynthSsgParams.h"
#include "../../Synth/Wavetable/SynthWtParams.h"
#include "../../Synth/Wt2/SynthWt2Params.h"
#include "../../Synth/WtPlus/SynthWtPlusParams.h"
#include "../../Synth/Rhythm/SynthRhythmParams.h"
#include "../../Synth/Adpcm/SynthAdpcmParams.h"
#include "../../Synth/AdpcmPlus/SynthAdpcmPlusParams.h"
#include "../../Synth/Beep/SynthBeepParams.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"

struct SynthCoreParams
{
    // --- Monophonic Mode ---
    bool monoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    OpnaParams opna;
    OpnParams opn;
    OplParams opl;
    Opl3Params opl3;
    OpmParams opm;
    Opzx7Params opzx7;
    SsgParams ssg;
    WtParams wt;
    Wt2Params wt2;
    WtPlusParams wtPlus;
    RhythmParams rhythm;
    AdpcmParams adpcm;
    AdpcmPlusParams adpcmPlus;
    BeepParams beep;
};
