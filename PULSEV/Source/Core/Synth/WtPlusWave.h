#pragma once

#include <array>
#include <vector>

#include "../Const/ConstGlobal.h"

// WT+ の波形メモリ 1 スロットぶん。
// 実データはプロセッサが所有し、音源コアはポインタ越しに読むだけ。
// 32 スロットぶんの実データをパラメータで持つと数が膨大になるため、
// state には相対パスだけを保存してファイルから読み直す方式にしている。
struct WtPlusWaveSlot {
    // Global::WtPlus::waveResolution 点へ展開したもの。未ロードなら空。
    std::vector<float> data;

    // 読み込んだファイル本来のサンプル数 (32 / 64 / 128 / 256)。
    // 再生時のテーブルサイズに使うので、階段の粗さがファイルどおりになる。
    int sampleCount = 0;
};

using WtPlusWaveSlots = std::array<WtPlusWaveSlot, Global::WtPlus::slots>;
