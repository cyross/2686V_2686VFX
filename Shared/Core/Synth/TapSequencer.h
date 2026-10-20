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
//
// LOOP (4.1.0) が入っていれば、表のループの組 (START・END・COUNT) に沿って進む
// (StepLoop.h)。最後の段を使ったら 1 段目へ戻り、ループも数え直す。ループの
// 進み具合も cursor と同じくチャンネルで 1 つ。
struct TapSeqParams
{
    bool enable = false;
    int steps = 1;
    bool loop = false;
    StepTable* table = nullptr;
};

// 段 s (0 から) を使ったあと、次に使う段 (0 から) を返す
inline int tapSeqNext(const TapSeqParams& p, int s)
{
    const int steps = std::clamp(p.steps, 1, StepTable::size);

    if (!p.loop || p.table == nullptr) return (s + 1) % steps;

    const int next = StepLoop::next(p.table->loopView(), p.table->loopCount(), steps, p.table->tapLoops, s + 1);

    if (next > steps) {
        p.table->tapLoops.clear();
        return 0;
    }

    return next - 1;
}

// 1 段目へ戻す。ループも数え直す
inline void tapSeqRewind(StepTable& table)
{
    table.cursor.store(0, std::memory_order_relaxed);
    table.tapLoops.clear();
}

// 押したときの値を取り出し、次の段へ進める。切れていれば neutral を返す。
inline float tapSeqTake(const TapSeqParams& p, float neutral)
{
    if (!p.enable || p.table == nullptr) return neutral;

    const int steps = std::clamp(p.steps, 1, StepTable::size);

    int s = p.table->cursor.load(std::memory_order_relaxed);

    if (s < 0 || s >= steps) s = 0;

    const float value = p.table->level(s + 1);

    p.table->cursor.store(tapSeqNext(p, s), std::memory_order_relaxed);

    return value;
}
