#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"

namespace
{
    constexpr double kRate = 48000.0;

    SsgHwEnvParams makeParams(int shape, float period = 10.0f)
    {
        SsgHwEnvParams p;
        p.enable = true;
        p.shape = shape;
        p.period = period;
        p.min = 0.0f;
        p.max = 1.0f;
        p.smooth = false;
        return p;
    }

    std::vector<float> run(int shape, int samples, float period = 10.0f)
    {
        SsgHwEnv env;
        env.prepare(kRate);
        env.setParameters(makeParams(shape, period));
        env.noteOn();

        std::vector<float> out;
        out.reserve((size_t)samples);

        for (int i = 0; i < samples; ++i) {
            out.push_back(env.process());
        }

        return out;
    }
}

// SsgHwEnv : 全 44 波形が min〜max に収まる
TEST_CASE("SsgHwEnv: all 44 shapes stay within min..max")
{
    // 出力はゲインとして掛けられるので、範囲を外すと歪む。
    for (int shape = 0; shape < (int)SsgHwShape::Size; ++shape) {
        auto v = run(shape, 4800);

        float mn = 2.0f;
        float mx = -1.0f;

        for (float x : v) {
            mn = std::fmin(mn, x);
            mx = std::fmax(mx, x);
        }

        INFO("shape = " << shape);

        CHECK(mn >= -1.0e-4f);
        CHECK(mx <= 1.0f + 1.0e-4f);
    }
}

// SsgHwEnv : 全 44 波形が NaN / Inf を出さない
TEST_CASE("SsgHwEnv: all 44 shapes stay finite")
{
    for (int shape = 0; shape < (int)SsgHwShape::Size; ++shape) {
        auto v = run(shape, 2400);

        INFO("shape = " << shape);

        for (float x : v) {
            REQUIRE(std::isfinite(x));
        }
    }
}

// SsgHwEnv : min / max の指定が出力範囲になる
TEST_CASE("SsgHwEnv: min and max bound the output")
{
    for (int shape = 0; shape < (int)SsgHwShape::Size; ++shape) {
        SsgHwEnv env;
        env.prepare(kRate);

        auto p = makeParams(shape);
        p.min = 0.25f;
        p.max = 0.75f;

        env.setParameters(p);
        env.noteOn();

        INFO("shape = " << shape);

        for (int i = 0; i < 4800; ++i) {
            float x = env.process();

            CHECK(x >= 0.25f - 1.0e-4f);
            CHECK(x <= 0.75f + 1.0e-4f);
        }
    }
}

// SsgHwEnv : 保持形は途中で動きが止まる
TEST_CASE("SsgHwEnv: hold shapes settle and stop moving")
{
    // 実機 shape 9/11/13/15 (= index 1/3/5/7) は 1 周したら保持する。
    // 判定は「Square75 より手前で、かつ奇数」。
    for (int shape : { 1, 3, 5, 7 }) {
        auto v = run(shape, 48000, 4.0f);

        INFO("shape = " << shape);

        // 後半はまったく動かないはず
        float last = v.back();

        for (size_t i = v.size() * 3 / 4; i < v.size(); ++i) {
            CHECK(v[i] == doctest::Approx(last));
        }
    }
}

// SsgHwEnv : 繰り返し形は動き続ける
TEST_CASE("SsgHwEnv: repeating shapes keep moving")
{
    // 実機 shape 8/10/12/14 (= index 0/2/4/6) は繰り返す。
    for (int shape : { 0, 2, 4, 6 }) {
        auto v = run(shape, 48000, 4.0f);

        INFO("shape = " << shape);

        float mn = 2.0f;
        float mx = -1.0f;

        for (size_t i = v.size() * 3 / 4; i < v.size(); ++i) {
            mn = std::fmin(mn, v[i]);
            mx = std::fmax(mx, v[i]);
        }

        CHECK(mx - mn > 0.1f);   // 後半でも振れている
    }
}

// SsgHwEnv : 無効化すると 1.0 を返す
TEST_CASE("SsgHwEnv: disabled returns 1.0")
{
    SsgHwEnv env;
    env.prepare(kRate);

    auto p = makeParams(0);
    p.enable = false;

    env.setParameters(p);
    env.noteOn();

    for (int i = 0; i < 100; ++i) {
        CHECK(env.process() == doctest::Approx(1.0f));
    }
}

// SsgHwEnv : 同じ設定なら再現性がある
TEST_CASE("SsgHwEnv: same settings give the same output")
{
    // Sample & Hold / Random は乱数を使うが、noteOn ごとに同じ種から
    // 始まる作りなので、2 回走らせても同じ結果になる。
    for (int shape = 0; shape < (int)SsgHwShape::Size; ++shape) {
        auto a = run(shape, 2400);
        auto b = run(shape, 2400);

        INFO("shape = " << shape);

        REQUIRE(a.size() == b.size());

        for (size_t i = 0; i < a.size(); ++i) {
            REQUIRE(a[i] == doctest::Approx(b[i]));
        }
    }
}
