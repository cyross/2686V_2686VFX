#include "doctest/doctest.h"

#include <cmath>
#include <string>
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

TEST_CASE("StepEnv: STRICT R を入れると、前の段と同じ値の段も R 秒ぶん保つ")
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
        p.strictRate = true;

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

TEST_CASE("StepEnv: STRICT R を切ると、前の段と同じ値の段はすぐ終わる (4.2.x までの鳴り方)")
{
    StepTable t(0.0f, 0.0f);
    t.setRate(1, 0.1f);   // 100 サンプル
    t.setRate(2, 0.1f);
    t.setRate(3, 0.1f);
    fill(t, { 1.0f, 1.0f, 0.0f });

    auto p = makeParams(t, 3);

    REQUIRE_FALSE(p.strictRate);

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();

    float v = 0.0f;

    // 段 1 で 1 へ着くと段 2 はすぐ終わり、段 3 で 0 へ向かう。200 サンプルで走り終える
    for (int i = 0; i < 150; ++i) v = env.process();

    CHECK(v == doctest::Approx(0.5f).epsilon(0.05));

    for (int i = 0; i < 60; ++i) v = env.process();

    CHECK(v == doctest::Approx(0.0f));
}

TEST_CASE("StepEnv: STRICT R を入れると、リリースも RR 秒を待ってから ENDL へ移る")
{
    StepTable t(0.0f, 0.0f);
    fill(t, { 0.5f });

    for (bool strict : { false, true }) {
        CAPTURE(strict);

        auto p = makeParams(t, 1);
        p.lr = 0.5f;          // 最後の段と同じ値
        p.rr = 0.1f;          // 100 サンプル
        p.endlEnable = true;
        p.endl = 0.0f;
        p.strictRate = strict;

        StepEnv env;
        env.prepare(0, -1, kRate);
        env.setParameters(p);
        env.noteOn();
        env.process();
        env.noteOff();

        float v = 0.0f;

        for (int i = 0; i < 50; ++i) v = env.process();

        // 切っていれば LR に着いていて、すぐ ENDL へ落ちる。入れていれば RR のあいだ LR を保つ
        if (strict) CHECK(v == doctest::Approx(0.5f));
        else CHECK(v == doctest::Approx(0.0f));

        for (int i = 0; i < 60; ++i) v = env.process();

        CHECK(v == doctest::Approx(0.0f));
        CHECK(env.isIdle());
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

    CHECK(t.live.read().phase == 1);

    for (int i = 0; i < 50; ++i) a.process();

    CHECK(t.live.read().phase == 1);
    CHECK(t.live.read().frac == doctest::Approx(0.5f).epsilon(0.05));

    for (int i = 0; i < 60; ++i) a.process();

    CHECK(t.live.read().phase == 2);

    // 最後の段を走り終えたら、保っている (サスティン)
    for (int i = 0; i < 100; ++i) a.process();

    CHECK(t.live.read().phase == LivePosition::sustain);

    // あとから押した音が持ち主になる。先の音はもう書かない
    StepEnv b;
    b.prepare(0, -1, kRate);
    b.setParameters(p);
    b.noteOn();

    CHECK(t.live.read().phase == 1);

    for (int i = 0; i < 300; ++i) a.process();

    CHECK(t.live.read().phase == 1);

    // 離すとリリース、走り終えたら鳴っていない
    for (int i = 0; i < 50; ++i) b.process();

    b.noteOff();
    b.process();

    CHECK(t.live.read().phase == LivePosition::release);

    for (int i = 0; i < 100; ++i) b.process();

    CHECK(b.isIdle());
    CHECK(t.live.read().phase == LivePosition::idle);
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
    CHECK(t.live.read().phase == 1);
    CHECK(t.live.read().frac == doctest::Approx(0.5f).epsilon(0.05));
}

// ---------------------------------------------------------------- RATE の段 (4.3.0)
namespace
{
    // 段の L と RATE の進め方を入れる。R は打ち切りの時間 (0 なら打ち切らない)
    void rateStep(StepTable& t, int step, float level, float delta, float tickSec = 0.0f, float limit = 0.0f)
    {
        t.setLevel(step, level);
        t.setRate(step, limit);
        t.setRateStep(step, { true, delta, tickSec, 0 });
    }

    float run(StepEnv& env, int samples)
    {
        float v = env.value();

        for (int i = 0; i < samples; ++i) v = env.process();

        return v;
    }
}

TEST_CASE("StepEnv: RATE の段は、どこから始めても同じ速さ (D / 秒) で L へ向かい、行き過ぎない")
{
    StepTable t(0.0f, 0.0f);
    rateStep(t, 1, 1.0f, 10.0f);   // 1 秒あたり 10。0 から 1 まで 100 サンプル

    for (float stl : { 0.0f, 0.5f }) {
        CAPTURE(stl);

        auto p = makeParams(t, 1);
        p.stl = stl;

        StepEnv env;
        env.prepare(0, -1, kRate);
        env.setParameters(p);
        env.noteOn();

        // 50 サンプルで 0.5 動く
        CHECK(run(env, 50) == doctest::Approx(stl + 0.5f).epsilon(0.01));

        // 届いたら L を保つ
        CHECK(run(env, 200) == doctest::Approx(1.0f));
    }
}

TEST_CASE("StepEnv: RATE の刻みは、段に入ってから T 秒待つたびに D ずつ動く")
{
    StepTable t(0.0f, 0.0f);
    rateStep(t, 1, 1.0f, 0.25f, 0.01f);   // 10 サンプルごとに 0.25

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 1));
    env.noteOn();

    // 入った瞬間は動かない
    CHECK(run(env, 9) == doctest::Approx(0.0f));
    CHECK(run(env, 1) == doctest::Approx(0.25f));
    CHECK(run(env, 9) == doctest::Approx(0.25f));
    CHECK(run(env, 1) == doctest::Approx(0.5f));
    CHECK(run(env, 20) == doctest::Approx(1.0f));
    CHECK(run(env, 100) == doctest::Approx(1.0f));
}

TEST_CASE("StepEnv: RATE の段は R 秒で打ち切り、着いたところから次の段を始める")
{
    StepTable t(0.0f, 0.0f);
    rateStep(t, 1, 1.0f, 10.0f, 0.0f, 0.03f);   // 30 サンプルで 0.3 まで
    rateStep(t, 2, 1.0f, 10.0f);                // 続きを同じ速さで 1 まで (70 サンプル)

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 2));
    env.noteOn();

    CHECK(run(env, 30) == doctest::Approx(0.3f).epsilon(0.01));
    CHECK(run(env, 35) == doctest::Approx(0.65f).epsilon(0.02));
    CHECK(run(env, 100) == doctest::Approx(1.0f));
}

TEST_CASE("StepEnv: 打ち切った最後の段は、着いたところの値を保つ")
{
    StepTable t(0.0f, 0.0f);
    rateStep(t, 1, 1.0f, 0.25f, 0.01f, 0.025f);   // 25 サンプルで 2 刻み (0.5)

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(makeParams(t, 1));
    env.noteOn();

    CHECK(run(env, 200) == doctest::Approx(0.5f));
}

TEST_CASE("StepEnv: D が 0 で打ち切りもしない RATE の段は、離すまでその段にいる")
{
    StepTable t(0.0f, 0.0f);
    rateStep(t, 1, 1.0f, 0.0f);

    auto p = makeParams(t, 1);
    p.stl = 0.25f;
    p.rr = 0.01f;

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();

    CHECK(run(env, 1000) == doctest::Approx(0.25f));

    env.noteOff();
    run(env, 20);

    CHECK(env.isIdle());
}

TEST_CASE("StepEnv: RATE のリリースは D ずつ LR へ。D が 0 なら TIME と同じに RR 秒で")
{
    StepTable t(0.0f, 1.0f);   // 段の L は 1、R は 0 (すぐ着く)

    for (float releaseDelta : { 20.0f, 0.0f }) {
        CAPTURE(releaseDelta);

        t.setRateStep(StepTable::releaseStep, { true, releaseDelta, 0.0f, 0 });

        auto p = makeParams(t, 1);
        p.rr = 0.1f;   // TIME なら 100 サンプル。RATE (1 秒あたり 20) なら 50 サンプル

        StepEnv env;
        env.prepare(0, -1, kRate);
        env.setParameters(p);
        env.noteOn();
        run(env, 10);
        env.noteOff();

        if (releaseDelta > 0.0f) {
            CHECK(run(env, 25) == doctest::Approx(0.5f).epsilon(0.02));
            run(env, 30);
        }
        else {
            CHECK(run(env, 50) == doctest::Approx(0.5f).epsilon(0.02));
            run(env, 60);
        }

        CHECK(env.isIdle());
    }
}

TEST_CASE("StepEnv: START NOW なら、走っているあいだに押すとその時点の値から始める")
{
    StepTable t(0.0f, 0.0f);
    t.setRate(1, 0.1f);   // 0 から 1 へ 100 サンプル
    t.setLevel(1, 1.0f);

    for (bool now : { false, true }) {
        CAPTURE(now);

        auto p = makeParams(t, 1);
        p.startNow = now;

        StepEnv env;
        env.prepare(0, -1, kRate);
        env.setParameters(p);
        env.noteOn();

        const float mid = run(env, 50);

        REQUIRE(mid == doctest::Approx(0.5f).epsilon(0.02));

        env.noteOn();

        // 切っていれば STL (0) から、入れていれば 0.5 から 1 へ向かい直す
        const float first = env.process();

        if (now) CHECK(first == doctest::Approx(0.505f).epsilon(0.01));
        else CHECK(first == doctest::Approx(0.01f).epsilon(0.05));
    }
}

TEST_CASE("StepEnv: START NOW で止まっていれば、渡された値 (PARAM ENV のつまみ) から始める")
{
    StepTable t(0.0f, 0.0f);
    rateStep(t, 1, 1.0f, 10.0f);

    auto p = makeParams(t, 1);
    p.startNow = true;
    p.idleStart = 0.7f;

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();

    // 0.7 から 1 秒あたり 10 で 1 へ (30 サンプル)
    CHECK(run(env, 10) == doctest::Approx(0.8f).epsilon(0.01));
    CHECK(run(env, 30) == doctest::Approx(1.0f));
}

TEST_CASE("StepEnv: T SYNC なら、1 刻みはテンポに合わせた音符の長さ")
{
    StepTableStore store;
    auto& t = store.get("A_SSGSW256", 0.0f, 0.0f);

    t.setLevel(1, 1.0f);
    t.setRateStep(1, { true, 0.5f, 0.001f, 5 });   // 1/4 (1 拍)。T (秒) は T SYNC のあいだ使わない
    REQUIRE(std::string(StepRate::notes[5].label) == "1/4");

    store.bpm.store(60.0);   // 1 拍 1 秒 = 1000 サンプル

    auto p = makeParams(t, 1);
    p.tickSync = true;

    StepEnv env;
    env.prepare(0, -1, kRate);
    env.setParameters(p);
    env.noteOn();

    CHECK(run(env, 999) == doctest::Approx(0.0f));
    CHECK(run(env, 1) == doctest::Approx(0.5f));

    // テンポが変わると、次の刻みから追う (120 なら 500 サンプル)
    store.bpm.store(120.0);

    CHECK(run(env, 500) == doctest::Approx(1.0f));
}

TEST_CASE("StepRate: 段の長さと終わりの値は、音の側 (StepEnv) と同じ式で出す")
{
    // 刻み: 1 を 0.3 ずつ。4 刻みで届く
    auto seg = StepRate::segment(0.0f, 1.0f, 0.3f, 0.01, 0.0f);

    CHECK_FALSE(seg.endless);
    CHECK(seg.seconds == doctest::Approx(0.04));
    CHECK(seg.end == doctest::Approx(1.0f));

    // 打ち切り: 0.025 秒で 2 刻み
    seg = StepRate::segment(0.0f, 1.0f, 0.3f, 0.01, 0.025f);

    CHECK(seg.seconds == doctest::Approx(0.025));
    CHECK(seg.end == doctest::Approx(0.6f));

    // 下がる向き。線形で 1 秒あたり 2
    seg = StepRate::segment(1.0f, 0.0f, 2.0f, 0.0, 0.0f);

    CHECK(seg.seconds == doctest::Approx(0.5));
    CHECK(StepRate::valueAt(1.0f, 0.0f, 2.0f, 0.0, 0.25) == doctest::Approx(0.5f));

    // 動かず打ち切りもしない
    CHECK(StepRate::segment(0.0f, 1.0f, 0.0f, 0.0, 0.0f).endless);
}
