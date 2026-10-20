#include "doctest/doctest.h"

#include <optional>

#include "Shared/Core/Synth/TapSequencer.h"

TEST_CASE("TapSeq: 押すたびに段を進め、STEPS で折り返す。cursor は鳴らした段")
{
    StepTable table(0.0f, 1.0f);
    table.setLevel(1, 0.5f);
    table.setLevel(2, 2.0f);
    table.setLevel(3, 4.0f);

    TapSeqParams p;
    p.enable = true;
    p.steps = 3;
    p.table = &table;

    // 押す前は開始前
    CHECK(table.cursor.load() < 0);

    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.5f));
    CHECK(table.cursor.load() == 0);
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(2.0f));
    CHECK(table.cursor.load() == 1);
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(4.0f));
    CHECK(table.cursor.load() == 2);

    // 3 段で 1 段目へ戻る
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.5f));
    CHECK(table.cursor.load() == 0);

    // CLEAR STEP で開始前へ。次に押すと 1 段目
    tapSeqTake(p, 1.0f);
    tapSeqRewind(table);

    CHECK(table.cursor.load() < 0);
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.5f));
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
    CHECK(table.cursor.load() == 0);
}

TEST_CASE("TapSeq: SKIP の段は進めたうえで補正しない")
{
    StepTable table(0.0f, 1.0f);
    table.setLevel(1, 0.5f);
    table.setLevel(2, 2.0f);
    table.setLevel(3, 4.0f);
    table.setSkip(2, true);

    TapSeqParams p;
    p.enable = true;
    p.steps = 3;
    p.table = &table;

    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(0.5f));

    // 2 段目は SKIP。段は 2 段目へ進み、値は補正なし
    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(1.0f));
    CHECK(table.cursor.load() == 1);

    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(4.0f));

    // 外せば値で補正する
    table.setSkip(2, false);
    tapSeqRewind(table);
    tapSeqTake(p, 1.0f);

    CHECK(tapSeqTake(p, 1.0f) == doctest::Approx(2.0f));
}

TEST_CASE("TapSeq: INCREMENT / DECREMENT は開始前から 1 段目・最後の段へ動かす")
{
    StepTable table(0.0f, 1.0f);

    TapSeqParams p;
    p.enable = true;
    p.steps = 4;
    p.table = &table;

    CHECK(tapSeqForward(p, -1) == 0);
    CHECK(tapSeqForward(p, 0) == 1);
    CHECK(tapSeqForward(p, 3) == 0);

    CHECK(tapSeqBack(4, -1) == 3);
    CHECK(tapSeqBack(4, 0) == 3);
    CHECK(tapSeqBack(4, 2) == 1);
}

namespace
{
    // 再生しているかだけを返す、DAW (juce::AudioPlayHead) の代わり
    struct FakePlayHead
    {
        struct Position
        {
            bool playing = false;

            bool getIsPlaying() const { return playing; }
        };

        bool playing = false;

        std::optional<Position> getPosition() const { return Position{ playing }; }
    };
}

TEST_CASE("TapSeq: DAW の再生を止めたら、すべて開始前へ戻す")
{
    StepTableStore store;
    auto& amp = store.get("A_TAPAMP", 0.0f, 1.0f);
    auto& pitch = store.get("A_TAPPITCH", 0.0f, 0.0f);

    FakePlayHead head;

    head.playing = true;
    store.followTransport(&head);

    amp.cursor.store(2);
    pitch.cursor.store(5);

    // 再生しているあいだは動かさない
    store.followTransport(&head);

    CHECK(amp.cursor.load() == 2);

    head.playing = false;
    store.followTransport(&head);

    CHECK(amp.cursor.load() < 0);
    CHECK(pitch.cursor.load() < 0);

    // 止まったままなら、そのあと押して進めた段は戻さない
    amp.cursor.store(1);
    store.followTransport(&head);
    store.followTransport<FakePlayHead>(nullptr);

    CHECK(amp.cursor.load() == 1);
}
