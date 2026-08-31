#pragma once

#include <array>
#include <map>

#include <JuceHeader.h>

#include "../Const/ConstGlobal.h"

// ============================================================================
// WT PITCH MOD の変調波形 (HuC6280 モード)
// ============================================================================
// 実機は「もう 1 本のチャンネルの波形メモリ 32 サンプル」を変調に使う。
// ここではそれを複数枚まで持てるようにして、演奏中に切り替えられる。
//
// 32 サンプル × 枚数をチャンネルごとにパラメータで持つと数が膨大になるため、
// WT+ と同じく実データはプロセッサが所有し、state には相対パスだけを保存する。
// パラメータとして残すのは「どのスロットを使うか」だけ。
struct WtModWaveSlot {
    // -1.0〜1.0 の 32 サンプル。読み込んでいなければ hasData が false。
    std::array<float, Global::WtMod::waveSize> data = { 0.0f };
    bool hasData = false;
};

using WtModWaveSlots = std::array<WtModWaveSlot, Global::WtMod::slots>;

// チャンネル (APVTS のプレフィックス) ごとの持ち分。
//
// std::map は要素の参照が安定しているので、音源側は一度引いたポインタを
// 持ち続けてよい。プラグインの実体ごとに 1 つ持つため、グローバルには置かない。
using WtModWaveStore = std::map<juce::String, WtModWaveSlots>;
