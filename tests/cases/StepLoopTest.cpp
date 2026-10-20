#include "doctest/doctest.h"

#include <vector>

#include "Shared/Core/Synth/StepLoop.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Synth/TapSequencer.h"
#include "Shared/Effect/Envelope/Step/EnvStep.h"

namespace
{
    using Entries = std::vector<StepLoop::Entry>;

    // 1 段目から、通る段を順に並べる。STEPS を越えるか limit に達したら止める
    std::vector<int> walk(Entries loops, int steps, int limit = 64)
    {
        StepLoop::sort(loops);

        StepLoop::State st;
        std::vector<int> path;

        for (int s = 1; s <= steps && (int)path.size() < limit;) {
            path.push_back(s);
            s = StepLoop::next(loops, (int)loops.size(), steps, st, s);
        }

        return path;
    }
}

TEST_CASE("StepLoop: END の段を終えたら START へ戻る。COUNT 回戻る")
{
    // 2〜3 を 2 回戻る (3 回通る)
    CHECK(walk({ { 2, 3, 2 } }, 4) == std::vector<int>{ 1, 2, 3, 2, 3, 2, 3, 4 });

    // 1 段だけのループ
    CHECK(walk({ { 2, 2, 1 } }, 3) == std::vector<int>{ 1, 2, 2, 3 });
}

TEST_CASE("StepLoop: COUNT 0 は無限")
{
    const auto path = walk({ { 2, 3, 0 } }, 4, 9);

    CHECK(path == std::vector<int>{ 1, 2, 3, 2, 3, 2, 3, 2, 3 });
}

TEST_CASE("StepLoop: 入れ子は内側を回し終えてから外側が戻り、戻るたびに内側を数え直す")
{
    // 外 1〜4 を 1 回、内 2〜3 を 1 回
    CHECK(walk({ { 1, 4, 1 }, { 2, 3, 1 } }, 4)
        == std::vector<int>{ 1, 2, 3, 2, 3, 4, 1, 2, 3, 2, 3, 4 });

    // END が同じ入れ子。内 3〜4 を回し終えたら外 1〜4 が戻る
    CHECK(walk({ { 1, 4, 1 }, { 3, 4, 1 } }, 5)
        == std::vector<int>{ 1, 2, 3, 4, 3, 4, 1, 2, 3, 4, 3, 4, 5 });

    // START が同じ入れ子。END が先のほうが内側
    CHECK(walk({ { 1, 2, 1 }, { 1, 3, 1 } }, 3)
        == std::vector<int>{ 1, 2, 1, 2, 3, 1, 2, 1, 2, 3 });
}

TEST_CASE("StepLoop: END を跨いだら、先のループを回し終えて次のループの START へ進む")
{
    // A 1〜3 を 1 回、B 2〜5 を 1 回。A を回し終えたら B の START (2) へ
    CHECK(walk({ { 1, 3, 1 }, { 2, 5, 1 } }, 6)
        == std::vector<int>{ 1, 2, 3, 1, 2, 3, 2, 3, 4, 5, 2, 3, 4, 5, 6 });
}

TEST_CASE("StepLoop: STEPS より後ろへかかる組は効かない")
{
    CHECK(walk({ { 2, 5, 1 } }, 4) == std::vector<int>{ 1, 2, 3, 4 });
}

TEST_CASE("StepLoop: 入れ子の内側や跨がれている側を無限にしたら印を付ける")
{
    Entries nested{ { 1, 4, 1 }, { 2, 3, 0 } };

    CHECK_FALSE(StepLoop::isTrapped(nested, 2, 4, 0));
    CHECK(StepLoop::isTrapped(nested, 2, 4, 1));

    Entries crossed{ { 1, 3, 0 }, { 2, 5, 1 } };

    CHECK(StepLoop::isTrapped(crossed, 2, 6, 0));
    CHECK_FALSE(StepLoop::isTrapped(crossed, 2, 6, 1));

    // 外側の無限はよい
    Entries outer{ { 1, 4, 0 }, { 2, 3, 1 } };

    CHECK_FALSE(StepLoop::isTrapped(outer, 2, 4, 0));
}

TEST_CASE("StepTable: ループの組は並べて持ち、文字へ書き出して読み戻せる")
{
    StepTable a(0.0f, 0.0f);
    a.setLoops({ { 5, 8, 3 }, { 2, 4, 0 }, { 9, 6, 1 } });

    REQUIRE(a.loopCount() == 3);

    // START・END の昇順。START が END より後なら入れ替える
    CHECK(a.loopAt(0) == StepLoop::Entry{ 2, 4, 0 });
    CHECK(a.loopAt(1) == StepLoop::Entry{ 5, 8, 3 });
    CHECK(a.loopAt(2) == StepLoop::Entry{ 6, 9, 1 });

    a.reset();

    CHECK(a.loopCount() == 0);
}

TEST_CASE("StepEnv: Rich Mode はループの組で段を戻る")
{
    StepTable t(0.0f, 0.0f);

    for (int s = 1; s <= 3; ++s) t.setLevel(s, 0.1f * (float)s);

    t.setLoops({ { 2, 3, 1 } });

    StepEnvParams p;
    p.bypass = false;
    p.steps = 3;
    p.loop = true;
    p.loopMode = StepLoop::Rich;
    p.table = &t;

    StepEnv env;
    env.prepare(0, -1, 1000.0);
    env.setParameters(p);
    env.noteOn();

    // R は 0 なので、1 サンプルに 1 段ずつ進む: 1, 2, 3, 2, 3 のあと 3 を保つ
    std::vector<float> got;

    for (int i = 0; i < 7; ++i) got.push_back(env.process());

    const std::vector<float> want{ 0.1f, 0.2f, 0.3f, 0.2f, 0.3f, 0.3f, 0.3f };

    for (size_t i = 0; i < want.size(); ++i) CHECK(got[i] == doctest::Approx(want[i]));
}

TEST_CASE("StepEnv: LOOP が切れていれば Rich Mode の組は効かない")
{
    StepTable t(0.0f, 0.0f);

    for (int s = 1; s <= 3; ++s) t.setLevel(s, 0.1f * (float)s);

    t.setLoops({ { 1, 2, 0 } });

    StepEnvParams p;
    p.bypass = false;
    p.steps = 3;
    p.loop = false;
    p.loopMode = StepLoop::Rich;
    p.table = &t;

    StepEnv env;
    env.prepare(0, -1, 1000.0);
    env.setParameters(p);
    env.noteOn();

    CHECK(env.process() == doctest::Approx(0.1f));
    CHECK(env.process() == doctest::Approx(0.2f));
    CHECK(env.process() == doctest::Approx(0.3f));
    CHECK(env.process() == doctest::Approx(0.3f));
}

TEST_CASE("TapSeq: LOOP が入っていれば組に沿って進み、最後の段で 1 段目へ戻る")
{
    StepTable table(0.0f, 0.0f);

    for (int s = 1; s <= 3; ++s) table.setLevel(s, (float)s);

    table.setLoops({ { 2, 2, 1 } });

    TapSeqParams p;
    p.enable = true;
    p.steps = 3;
    p.loop = true;
    p.table = &table;

    std::vector<float> got;

    for (int i = 0; i < 8; ++i) got.push_back(tapSeqTake(p, 0.0f));

    // 1, 2, 2, 3 を繰り返す (戻った回数は 1 段目へ戻るたびに数え直す)
    CHECK(got == std::vector<float>{ 1, 2, 2, 3, 1, 2, 2, 3 });
}

TEST_CASE("TapSeq: LOOP が切れていれば組は効かない。RESET でループも数え直す")
{
    StepTable table(0.0f, 0.0f);

    for (int s = 1; s <= 3; ++s) table.setLevel(s, (float)s);

    table.setLoops({ { 1, 2, 2 } });

    TapSeqParams p;
    p.enable = true;
    p.steps = 3;
    p.loop = false;
    p.table = &table;

    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(1.0f));
    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(2.0f));
    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(3.0f));

    p.loop = true;
    tapSeqRewind(table);

    std::vector<float> got;

    for (int i = 0; i < 4; ++i) got.push_back(tapSeqTake(p, 0.0f));

    CHECK(got == std::vector<float>{ 1, 2, 1, 2 });

    tapSeqRewind(table);

    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(1.0f));
    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(2.0f));
    CHECK(tapSeqTake(p, 0.0f) == doctest::Approx(1.0f));
}
