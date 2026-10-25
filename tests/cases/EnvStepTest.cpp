#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "Shared/Effect/Envelope/Step/EnvStep.h"
#include "Shared/Core/Synth/StepTable.h"

namespace
{
    constexpr double kRate = 1000.0;

    // R を 0 (待たない) にした表へ、L を入れる。表は原子を持つので写せない。
    void fill(StepTable& t, std::initializer_list<float> levels)
    {
        int s = 1;

        for (float l : levels) t.setLevel(s++, l);
    }

    StepEnvParams makeParams(const StepTable& t, int steps)
    {
        StepEnvParams p;

        p.bypass = false;
        p.steps = steps;
        p.table = &t;

        return p;
    }
}

TEST_CASE("StepEnv: 表の段を順に進み、最後の段を保つ")
{
    StepTable t(0.0f, 0.0f);
    fill(t, { 0.25f, 0.5f, 0.75f });

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 3));
    env.noteOn();

    CHECK(env.process() == doctest::Approx(0.25f));
    CHECK(env.process() == doctest::Approx(0.5f));
    CHECK(env.process() == doctest::Approx(0.75f));

    for (int i = 0; i < 10; ++i) CHECK(env.process() == doctest::Approx(0.75f));
}

TEST_CASE("StepEnv: R 秒かけて次の段へ進む")
{
    StepTable t(0.0f, 0.0f);
    t.setRate(1, 0.1f);   // 100 サンプル
    t.setLevel(1, 1.0f);

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 1));
    env.noteOn();

    float v = 0.0f;

    for (int i = 0; i < 50; ++i) v = env.process();

    CHECK(v == doctest::Approx(0.5f).epsilon(0.02));

    for (int i = 0; i < 60; ++i) v = env.process();

    CHECK(v == doctest::Approx(1.0f));
}

TEST_CASE("StepEnv: 前の段と同じ値の段も、R 秒ぶん保つ")
{
    StepTable t(0.0f, 0.0f);
    t.setRate(1, 0.1f);   // 100 サンプル
    t.setRate(2, 0.1f);
    t.setRate(3, 0.1f);
    fill(t, { 1.0f, 1.0f, 0.0f });

    for (bool keep : { false, true }) {
        CAPTURE(keep);

        auto p = makeParams(t, 3);
        p.keep = keep;

        StepEnv env;
        env.prepare(0, -1, kRate);
        env.setParameters(p);
        env.noteOn();

        float v = 0.0f;

        // 段 1 (0 から 1 へ) と段 2 (1 のまま) で 200 サンプル。段 2 の終わりまで 1 を保つ
        for (int i = 0; i < 190; ++i) v = env.process();

        CHECK(v == doctest::Approx(1.0f));

        // 段 3 で 0 へ。KEEP なら段の頭の 1 を保ち、走り終えて 0 になる
        for (int i = 0; i < 60; ++i) v = env.process();

        if (keep) CHECK(v == doctest::Approx(1.0f));
        else CHECK(v == doctest::Approx(0.5f).epsilon(0.05));

        for (int i = 0; i < 60; ++i) v = env.process();

        CHECK(v == doctest::Approx(0.0f));
    }
}

TEST_CASE("StepEnv: 離すと RR 秒で LR へ。Use Endl なら ENDL を保つ")
{
    StepTable t(0.0f, 0.0f);
    fill(t, { 1.0f });

    auto p = makeParams(t, 1);
    p.lr = 0.2f;
    p.rr = 0.0f;

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();
    env.process();
    env.noteOff();

    CHECK(env.process() == doctest::Approx(0.2f));
    CHECK(env.isIdle());
    CHECK(env.process() == doctest::Approx(0.2f));

    p.endlEnable = true;
    p.endl = 0.6f;
    env.setParameters(p);
    env.noteOn();
    env.process();
    env.noteOff();

    CHECK(env.process() == doctest::Approx(0.6f));
}

TEST_CASE("StepEnv: LOOP は L.TO の段へ戻り、CNT 回で抜ける")
{
    StepTable t(0.0f, 0.0f);
    fill(t, { 0.1f, 0.2f, 0.3f });

    auto p = makeParams(t, 3);
    p.loop = true;
    p.loopTo = 1;
    p.loopCount = 1;

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();

    // 1 段目, 2 段目, 3 段目は L.TO (L1) へ戻る, 2 段目, 3 段目 (抜ける)
    CHECK(env.process() == doctest::Approx(0.1f));
    CHECK(env.process() == doctest::Approx(0.2f));
    CHECK(env.process() == doctest::Approx(0.1f));
    CHECK(env.process() == doctest::Approx(0.2f));
    CHECK(env.process() == doctest::Approx(0.3f));
    CHECK(env.process() == doctest::Approx(0.3f));
}

TEST_CASE("StepEnv: KEEP は段の頭の値を保ち、最後の段まで来たらその値を保つ")
{
    StepTable t(0.0f, 0.0f);
    t.setRate(1, 0.01f);
    t.setLevel(1, 1.0f);
    t.setRate(2, 0.01f);
    t.setLevel(2, 0.5f);

    auto p = makeParams(t, 2);
    p.stl = 0.25f;
    p.keep = true;

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();

    // 1 段目のあいだは STL
    CHECK(env.process() == doctest::Approx(0.25f));

    for (int i = 0; i < 15; ++i) env.process();

    // 2 段目のあいだは L1
    CHECK(env.process() == doctest::Approx(1.0f));

    // 最後の段を走り終えたら、離すまで L2 を保つ (サステイン)
    for (int i = 0; i < 30; ++i) env.process();

    CHECK(env.process() == doctest::Approx(0.5f));
}

TEST_CASE("StepEnv: 最後の段の値を保ち、離すとその値から RR 秒で LR へ向かう")
{
    for (const bool keep : { false, true }) {
        CAPTURE(keep);

        StepTable t(0.0f, 0.0f);

        for (int s = 1; s <= 4; ++s) {
            t.setRate(s, 0.01f);   // 10 サンプル
            t.setLevel(s, 0.25f * (float)s);
        }

        auto p = makeParams(t, 4);
        p.keep = keep;
        p.rr = 0.1f;               // 100 サンプル
        p.lr = 0.0f;

        StepEnv env;
        env.prepare(0, -1, kRate);
        env.setParameters(p);
        env.noteOn();

        float v = 0.0f;

        for (int i = 0; i < 200; ++i) v = env.process();

        // STEPS = 4。L4 へ届いたあとは L4 のまま
        CHECK(v == doctest::Approx(1.0f));

        env.noteOff();

        for (int i = 0; i < 50; ++i) v = env.process();

        // KEEP でなければ斜めに下がり、KEEP なら走り終えるまで L4 を保つ
        if (keep) CHECK(v == doctest::Approx(1.0f));
        else CHECK(v == doctest::Approx(0.5f).epsilon(0.05));

        for (int i = 0; i < 60; ++i) v = env.process();

        CHECK(v == doctest::Approx(0.0f));
        CHECK(env.isIdle());
    }
}

TEST_CASE("StepEnv: 表を書き換えると、次の setParameters で写し直す")
{
    StepTable t(0.0f, 0.0f);
    t.setLevel(1, 0.3f);

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 1));
    env.noteOn();

    CHECK(env.process() == doctest::Approx(0.3f));

    t.setLevel(1, 0.9f);
    env.setParameters(makeParams(t, 1));

    CHECK(env.process() == doctest::Approx(0.9f));
}

TEST_CASE("StepEnv: PARAM ENV は鳴っている位置を表へ書き、いちばん新しく押した音が持ち主になる")
{
    const std::vector<float> defaults{ 0.0f, 0.0f };

    StepTable t(0.0f, 0.0f, false, &defaults);
    t.setRate(1, 0.1f);   // 100 サンプル
    t.setRate(2, 0.1f);
    t.setSetValue(0, 1, 1.0f);
    t.setSetValue(0, 2, 0.5f);

    auto p = makeParams(t, 2);
    p.levelSet = 0;
    p.rr = 0.05f;         // 50 サンプル

    StepEnv a;
    a.prepare(0, -1, kRate);
    a.setParameters(p);
    a.noteOn();

    CHECK(t.live.phase.load() == 1);

    for (int i = 0; i < 50; ++i) a.process();

    CHECK(t.live.phase.load() == 1);
    CHECK(t.live.frac.load() == doctest::Approx(0.5f).epsilon(0.05));

    for (int i = 0; i < 60; ++i) a.process();

    CHECK(t.live.phase.load() == 2);

    // 最後の段を走り終えたら、保っている (サスティン)
    for (int i = 0; i < 100; ++i) a.process();

    CHECK(t.live.phase.load() == LivePosition::sustain);

    // あとから押した音が持ち主になる。先の音はもう書かない
    StepEnv b;
    b.prepare(0, -1, kRate);
    b.setParameters(p);
    b.noteOn();

    CHECK(t.live.phase.load() == 1);

    for (int i = 0; i < 300; ++i) a.process();

    CHECK(t.live.phase.load() == 1);

    // 離すとリリース、走り終えたら鳴っていない
    for (int i = 0; i < 50; ++i) b.process();

    b.noteOff();
    b.process();

    CHECK(t.live.phase.load() == LivePosition::release);

    for (int i = 0; i < 100; ++i) b.process();

    CHECK(b.isIdle());
    CHECK(t.live.phase.load() == LivePosition::idle);
}

TEST_CASE("StepEnv: SSG SW ENV[256] など、どの段のエンベロープも鳴っている位置を書く")
{
    StepTable t(0.0f, 0.0f);
    t.setRate(1, 0.1f);
    t.setLevel(1, 1.0f);

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 1));
    env.noteOn();

    for (int i = 0; i < 50; ++i) env.process();

    CHECK(t.live.owner.load() != 0u);
    CHECK(t.live.phase.load() == 1);
    CHECK(t.live.frac.load() == doctest::Approx(0.5f).epsilon(0.05));
}
