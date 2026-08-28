#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"

namespace
{
    constexpr double kRate = 48000.0;
}

// SsgSwEnv : バイパス時は 1.0 を返す
TEST_CASE("SsgSwEnv: bypass flag is honoured")
{
    SsgSwEnv env;
    env.prepare(0, kRate);

    SsgSwEnvParams p;
    p.bypass = true;
    env.setParameters(p);

    CHECK(env.isBypass());
}

// SsgSwEnv : 出力が有限で 0 以上
TEST_CASE("SsgSwEnv: output stays finite and non-negative")
{
    SsgSwEnv env;
    env.prepare(0, kRate);

    SsgSwEnvParams p;
    p.bypass = false;
    env.setParameters(p);
    env.noteOn();

    for (int i = 0; i < 48000; ++i) {
        float v = env.process();

        REQUIRE(std::isfinite(v));
        REQUIRE(v >= -1.0e-4f);

        if (i == 24000) env.noteOff();
    }
}

// SsgSwEnv11 : 出力が有限で 0 以上
TEST_CASE("SsgSwEnv11: output stays finite and non-negative")
{
    SsgSwEnv11 env;
    env.prepare(0, kRate);

    SsgSwEnv11Params p;
    p.bypass = false;
    env.setParameters(p);
    env.noteOn();

    for (int i = 0; i < 48000; ++i) {
        float v = env.process();

        REQUIRE(std::isfinite(v));
        REQUIRE(v >= -1.0e-4f);

        if (i == 24000) env.noteOff();
    }
}

// SsgSwPEnv11 : バイパス時は位相増分を素通しする
TEST_CASE("SsgSwPEnv11: bypass passes the phase delta through")
{
    SsgSwPEnv11 env;
    env.prepare(0, kRate);

    SsgSwPEnv11Params p;
    p.bypass = true;
    env.setParameters(p);

    for (float d : { 0.001f, 0.01f, 0.1f, 1.0f }) {
        CHECK(env.process(d) == doctest::Approx(d));
    }
}

// SsgSwPEnv11 : process(1.0f) が周波数比そのものになる
TEST_CASE("SsgSwPEnv11: process(1.0f) yields the frequency ratio")
{
    // FM のチップ全体へ掛けるときに process(1.0f) を比として使っている。
    // 任意の位相増分に対して線形であることを確かめる。
    SsgSwPEnv11 a;
    a.prepare(0, kRate);

    SsgSwPEnv11 b;
    b.prepare(0, kRate);

    SsgSwPEnv11Params p;
    p.bypass = false;
    a.setParameters(p);
    b.setParameters(p);

    a.noteOn();
    b.noteOn();

    for (int i = 0; i < 4800; ++i) {
        float ratio = a.process(1.0f);
        float scaled = b.process(0.02f);

        REQUIRE(std::isfinite(ratio));
        REQUIRE(scaled == doctest::Approx(ratio * 0.02f).epsilon(0.0001));
    }
}

// SsgSwPEnv11 : 比が正で有限のまま
TEST_CASE("SsgSwPEnv11: ratio stays positive and finite")
{
    // 比が 0 以下になると位相が止まる / 逆行する。
    SsgSwPEnv11 env;
    env.prepare(0, kRate);

    SsgSwPEnv11Params p;
    p.bypass = false;
    env.setParameters(p);
    env.noteOn();

    for (int i = 0; i < 48000; ++i) {
        float r = env.process(1.0f);

        REQUIRE(std::isfinite(r));
        REQUIRE(r > 0.0f);

        if (i == 24000) env.noteOff();
    }
}
