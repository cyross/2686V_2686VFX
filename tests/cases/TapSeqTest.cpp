#include "doctest/doctest.h"

#include "Shared/Core/Synth/TapSequencer.h"

TEST_CASE("TapSeq: 押すたびに段を進め、STEPS で折り返す")
{
    StepTable table(0.0f, 1.0f);
    table.setLevel(1, 0.5f);
    table.setLevel(2, 2.0f);
    table.setLevel(3, 4.0f);

    TapSeqParams p;
    p.enable = true;
    p.steps = 3;
    p.table = &table;

    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.5f));
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(2.0f));
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(4.0f));

    // 3 段で 1 段目へ戻る
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.5f));
    CHECK(table.cursor.load() == 1);
}

TEST_CASE("TapSeq: STEPS が 1 なら毎回同じ値、切れていれば補正しない")
{
    StepTable table(0.0f, 0.0f);
    table.setLevel(1, 700.0f);

    TapSeqParams p;
    p.enable = true;
    p.steps = 1;
    p.table = &table;

    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(700.0f));
    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(700.0f));

    p.enable = false;

    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(0.0f));
}

TEST_CASE("TapSeq: STEPS を減らしたら、次の段は範囲の頭へ戻す")
{
    StepTable table(0.0f, 1.0f);
    table.setLevel(1, 0.25f);
    table.cursor.store(7);

    TapSeqParams p;
    p.enable = true;
    p.steps = 4;
    p.table = &table;

    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.25f));
    CHECK(table.cursor.load() == 1);
}
