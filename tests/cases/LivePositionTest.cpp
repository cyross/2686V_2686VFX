// ============================================================================
// 鳴っている位置 (グラフの縦線。4.3.0)
// ============================================================================
//   ・書き手は、区間に入ったときの音量から行き先までのどこにいるかで進みを出す
//   ・AMP ENV は、区間 (アタック・ディケイ・サスティン・リリース) と進みを書き、鳴り終えたら止まる
//   ・あとから押した音が持ち主になる。波形プレビューの計算 (Mute) では書かない
#include "doctest/doctest.h"

#include "Shared/Core/Synth/LivePosition.h"
#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"

namespace
{
    constexpr double kRate = 1000.0;

    AmpAdsrParams ampParams(LivePosition& live)
    {
        AmpAdsrParams p;

        p.stl = 0.0f;
        p.ar = 0.1f;   // 100 サンプル
        p.dr = 0.1f;
        p.sl = 0.5f;
        p.rr = 0.1f;
        p.live = &live;

        return p;
    }

    // 音量を noteOn の値から n サンプル進める
    float run(AmpAdsrEnv& env, float level, int n)
    {
        for (int i = 0; i < n; ++i) level = env.process(level);

        return level;
    }
}

TEST_CASE("LivePosition: 区間・進み・書いた回数を 1 つに詰めて読み書きする")
{
    LivePosition live;
    LiveWriter writer;

    writer.start(&live);

    writer.write(LivePosition::release, 0.75f);

    const auto a = live.read();

    CHECK(a.phase == LivePosition::release);
    CHECK(a.frac == doctest::Approx(0.75f).epsilon(1.0e-4));

    // 同じ区間・進みでも、書くたびに回数が進む (読む側はこれが止まったら消す)
    writer.write(LivePosition::release, 0.75f);

    CHECK(live.read().seq != a.seq);

    writer.write(200, 2.0f);

    CHECK(live.read().phase == 200);
    CHECK(live.read().frac == doctest::Approx(1.0f));
}

TEST_CASE("LiveLevelWriter: 区間の中の進みは、入ったときの音量から行き先までのどこにいるか")
{
    LivePosition live;
    LiveLevelWriter writer;

    writer.start(&live);

    REQUIRE(writer.active());

    writer.write(LivePosition::attack, 0.0f, 1.0f);
    writer.write(LivePosition::attack, 0.25f, 1.0f);

    CHECK(live.read().phase == LivePosition::attack);
    CHECK(live.read().frac == doctest::Approx(0.25f));

    // ディケイは 1 から SL (0.5) へ
    writer.write(LivePosition::decay, 1.0f, 0.5f);
    writer.write(LivePosition::decay, 0.75f, 0.5f);

    CHECK(live.read().phase == LivePosition::decay);
    CHECK(live.read().frac == doctest::Approx(0.5f));

    // 動かないサスティンは真ん中
    writer.write(LivePosition::sustain, 0.5f, 0.0f);

    CHECK(live.read().frac == doctest::Approx(0.5f));

    // 鳴り終えたら書いて手を離す
    writer.write(LivePosition::idle, 0.0f, 0.0f);

    CHECK(live.read().phase == LivePosition::idle);
    CHECK_FALSE(writer.active());
}

TEST_CASE("AMP ENV: 区間と進みを書き、鳴り終えたら止まる")
{
    LivePosition live;
    AmpAdsrEnv env;

    env.prepare(kRate);
    env.setParameters(ampParams(live));

    float level = env.noteOn();

    level = run(env, level, 50);

    CHECK(live.read().phase == LivePosition::attack);
    CHECK(live.read().frac == doctest::Approx(0.5f).epsilon(0.05));

    level = run(env, level, 100);

    CHECK(live.read().phase == LivePosition::decay);
    CHECK(live.read().frac == doctest::Approx(0.5f).epsilon(0.1));

    level = run(env, level, 200);

    CHECK(live.read().phase == LivePosition::sustain);
    CHECK(live.read().frac == doctest::Approx(0.5f));

    env.noteOff();
    level = run(env, level, 50);

    CHECK(live.read().phase == LivePosition::release);
    CHECK(live.read().frac == doctest::Approx(0.5f).epsilon(0.1));

    level = run(env, level, 100);

    CHECK(env.isIdle());
    CHECK(live.read().phase == LivePosition::idle);
}

TEST_CASE("AMP ENV: あとから押した音が持ち主になり、プレビューの計算では書かない")
{
    LivePosition live;
    AmpAdsrEnv a, b;

    a.prepare(kRate);
    b.prepare(kRate);
    a.setParameters(ampParams(live));
    b.setParameters(ampParams(live));

    float la = a.noteOn();

    la = run(a, la, 150);

    REQUIRE(live.read().phase == LivePosition::decay);

    float lb = b.noteOn();

    lb = run(b, lb, 10);

    CHECK(live.read().phase == LivePosition::attack);

    // 先の音はもう書かない
    la = run(a, la, 300);

    CHECK(live.read().phase == LivePosition::attack);

    // プレビューの計算 (Mute) で押した音は持ち主にならない
    {
        const LivePosition::Mute mute;

        AmpAdsrEnv preview;

        preview.prepare(kRate);
        preview.setParameters(ampParams(live));

        float lp = preview.noteOn();

        lp = run(preview, lp, 300);
    }

    CHECK(live.read().phase == LivePosition::attack);
}
