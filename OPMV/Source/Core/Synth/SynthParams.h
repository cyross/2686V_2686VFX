#pragma once

// ============================================================================
// このプラグインのパラメータ
// ============================================================================
// 音源へ渡すもの (SynthCoreParams、12 本で共有) に、どの音源を鳴らすか
// (mode) を足したもの。OscMode の番号はプラグインごとに違うので、ここに置く。
#include "Shared/Core/Synth/SynthCoreParams.h"

#include "./SynthMode.h"

struct SynthParams : SynthCoreParams
{
    // --- Synth Mode ---
    OscMode mode = OscMode::OPM;
};
