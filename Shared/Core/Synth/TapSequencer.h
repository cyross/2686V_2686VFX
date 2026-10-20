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
// 段の値と「いまの段」(cursor。最後に使った段) は表 (StepTable) が持つ。cursor は
// チャンネルで 1 つなので、どのボイスで押しても同じ並びを進む。cursor が負なら
// 開始前で、次に押すと 1 段目を使う。STEPS が 1 のときは、毎回同じ値で補正する。
//
// LOOP (4.1.0) が入っていれば、表のループの組 (START・END・COUNT) に沿って進む
// (StepLoop.h)。最後の段を使ったら 1 段目へ戻り、ループも数え直す。ループの
// 進み具合も cursor と同じくチャンネルで 1 つ。
//
// SKIP (4.1.0) の段では、段は進めるが補正しない (音量 1 倍・音程 0 セント)。
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

// いまの段 cursor の次に使う段 (0 から)。開始前 (負) や範囲の外なら 1 段目
inline int tapSeqForward(const TapSeqParams& p, int cursor)
{
    const int steps = std::clamp(p.steps, 1, StepTable::size);

    if (cursor < 0 || cursor >= steps) return 0;

    return tapSeqNext(p, cursor);
}

// いまの段を 1 つ戻す (DECREMENT)。開始前なら最後の段
inline int tapSeqBack(int steps, int cursor)
{
    steps = std::clamp(steps, 1, StepTable::size);

    if (cursor < 0 || cursor >= steps) return steps - 1;

    return (cursor + steps - 1) % steps;
}

// 開始前へ戻す。ループも数え直す
inline void tapSeqRewind(StepTable& table)
{
    table.clearCursor();
}

// 押したときに次の段へ進め、その段の値を取り出す。切れていれば neutral を返す。
// SKIP の段では、進めたうえで neutral を返す。
inline float tapSeqTake(const TapSeqParams& p, float neutral)
{
    if (!p.enable || p.table == nullptr) return neutral;

    const int s = tapSeqForward(p, p.table->cursor.load(std::memory_order_relaxed));

    p.table->cursor.store(s, std::memory_order_relaxed);

    if (p.table->skipped(s + 1)) return neutral;

    return p.table->level(s + 1);
}
