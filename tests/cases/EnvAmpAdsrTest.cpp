#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"

namespace
{
    constexpr double kRate = 48000.0;

    AmpAdsrParams makeParams(float stl, float ar, float dr, float sl, float rr)
    {
        AmpAdsrParams p;
        p.bypass = false;
        p.stl = stl;
        p.ar = ar;
        p.dr = dr;
        p.sl = sl;
        p.rr = rr;
        p.kor = false;
        return p;
    }

    // noteOn してから n サンプル進めた軌跡を返す
    std::vector<float> trajectory(AmpAdsrEnv& env, int samples)
    {
        std::vector<float> out;
        out.reserve((size_t)samples + 1);

        float level = env.noteOn();
        out.push_back(level);

        for (int i = 0; i < samples; ++i) {
            level = env.process(level);
            out.push_back(level);
        }

        return out;
    }
}

// AmpAdsrEnv : STL が発音開始レベルになる
TEST_CASE("AmpAdsrEnv: STL becomes the note-on level")
{
    // 以前 PrHelper::applyAdsrAmpEnv が stl を写しておらず、
    // STL つまみがまったく効いていなかった。ここが回帰の砦。
    for (float stl : { 0.0f, 0.25f, 0.5f, 0.9f }) {
        AmpAdsrEnv env;
        env.prepare(kRate);
        env.setParameters(makeParams(stl, 0.01f, 0.0f, 1.0f, 0.2f));

        CHECK(env.noteOn() == doctest::Approx(stl));
    }
}

// AmpAdsrEnv : アタックは AR の時間でちょうど 1.0 へ届く
TEST_CASE("AmpAdsrEnv: attack reaches 1.0 exactly at AR")
{
    // AR=10ms / 48kHz なら 480 サンプル
    AmpAdsrEnv env;
    env.prepare(kRate);
    env.setParameters(makeParams(0.0f, 0.01f, 0.0f, 1.0f, 0.2f));

    auto t = trajectory(env, 480);

    CHECK(t.front() == doctest::Approx(0.0f));
    CHECK(t[48] == doctest::Approx(0.1f).epsilon(0.001));    // 1ms で 10%
    CHECK(t[240] == doctest::Approx(0.5f).epsilon(0.001));   // 5ms で 50%
    CHECK(t[480] == doctest::Approx(1.0f));
}

// AmpAdsrEnv : STL を上げてもアタックの所要時間は変わらない
TEST_CASE("AmpAdsrEnv: STL does not change the attack duration")
{
    // attackInc が (1 - stl) でスケールされるため、開始レベルが違っても
    // 同じサンプル数で 1.0 へ届く。
    for (float stl : { 0.0f, 0.5f, 0.9f }) {
        AmpAdsrEnv env;
        env.prepare(kRate);
        env.setParameters(makeParams(stl, 0.01f, 0.0f, 1.0f, 0.2f));

        auto t = trajectory(env, 480);

        CHECK(t.front() == doctest::Approx(stl));
        CHECK(t[480] == doctest::Approx(1.0f));
    }
}

// AmpAdsrEnv : バイパス時は noteOn が 1.0 を返す
TEST_CASE("AmpAdsrEnv: bypassed noteOn returns 1.0")
{
    AmpAdsrEnv env;
    env.prepare(kRate);

    auto p = makeParams(0.5f, 0.01f, 0.0f, 1.0f, 0.2f);
    p.bypass = true;
    env.setParameters(p);

    CHECK(env.isBypass());
    CHECK(env.noteOn() == doctest::Approx(1.0f));
}

// AmpAdsrEnv : カーブパスと線形パスがアタックで一致する
TEST_CASE("AmpAdsrEnv: curve path matches linear path on attack")
{
    // 既定のカーブロジックは Logic::Linear (恒等写像) なので、CurveCore を
    // 繋いでも繋がなくてもアタックの軌跡は一致するはず。
    // 以前はカーブパスだけ 1 サンプル遅れていた。
    CurveCore curve;

    AmpAdsrEnv linear;
    linear.prepare(kRate);
    linear.setParameters(makeParams(0.0f, 0.01f, 0.0f, 1.0f, 0.2f));

    AmpAdsrEnv curved;
    curved.prepare(kRate);
    curved.setCurveCore(&curve);
    curved.setParameters(makeParams(0.0f, 0.01f, 0.0f, 1.0f, 0.2f));

    auto a = trajectory(linear, 480);
    auto b = trajectory(curved, 480);

    REQUIRE(a.size() == b.size());

    for (size_t i = 0; i < a.size(); ++i) {
        CHECK(a[i] == doctest::Approx(b[i]).epsilon(0.0001));
    }
}
