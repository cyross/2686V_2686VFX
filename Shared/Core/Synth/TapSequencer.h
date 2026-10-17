#pragma once

#include <algorithm>

#include "./StepTable.h"

// ============================================================================
// TAP AMP / PITCH SEQUENCER (音の側)
// ============================================================================
// 鍵を押すたびに段を 1 つ進め、その段の値で音量・音程を補正する。
// ドライバが音符ごとに音量や音程を少しずつ変えていた手法を、段の並びとして持つ。
//
//   TAP AMP   : 値は割合 (0〜4)。最後に決まった音量へ掛ける
//   TAP PITCH : 値はセント (-2400〜2400)。最後に決まった音程へ足す
//
// 段の値と「次に使う段」(cursor) は表 (StepTable) が持つ。cursor はチャンネルで
// 1 つなので、どのボイスで押しても同じ並びを進む。STEPS が 1 のときは、毎回
// 同じ値で補正する。
struct TapSeqParams
{
    bool enable = false;
    int steps = 1;
    StepTable* table = nullptr;
};

// 押したときの値を取り出し、次の段へ進める。切れていれば neutral を返す。
inline float tapSeqTake(const TapSeqParams& p, float neutral)
{
    if (!p.enable || p.table == nullptr) return neutral;

    const int steps = std::clamp(p.steps, 1, StepTable::size);

    int s = p.table->cursor.load(std::memory_order_relaxed);

    if (s < 0 || s >= steps) s = 0;

    const float value = p.table->level(s + 1);

    p.table->cursor.store((s + 1) % steps, std::memory_order_relaxed);

    return value;
}
