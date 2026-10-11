#pragma once

#include <array>

#include "Shared/Core/Const/ConstGlobal.h"

struct UnisonParams {
    int voices = 1;        // 1 to 8
    int detuneCents = 0;   // cents
    float spread = 1.0f;   // 0.0 to 1.0 (Stereo width)

    // 疑似高速アルペジオ
    bool arpEnable = false;
    int arpFreq = 60;      // Hz (1 to 4000)
    bool arpSmooth = true; // 低速時のクリック対策ランプ

    // ボイス単位の設定。添字がそのままボイスの番号になる。
    // ボイス0 (元の音) は 3.6.3 で足した。それまではボイス1〜7だけだった。
    std::array<float, Global::unisonVoices> paraDistance{};
    std::array<int, Global::unisonVoices> paraDetune{};
};
