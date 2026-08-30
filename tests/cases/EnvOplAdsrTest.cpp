#include "doctest/doctest.h"

#include <cmath>

#include "Effect/Envelope/Amp/OplAdsr/EnvOplAdsr.h"
#include "Core/Processor/ProcessorValues.h"

namespace
{
    constexpr double kRate = 48000.0;

    OplAdsrParams makeParams(int tl, int ksl = 0)
    {
        OplAdsrParams p;
        p.ar = 15;
        p.dr = 0;
        p.sl = 15;
        p.rr = 7;
        p.tl = tl;
        p.ksOPL.ksr = false;
        p.ksOPL.ksl = ksl;
        p.bypass = false;
        return p;
    }

    OplAdsr make(int tl, int ksl = 0)
    {
        OplAdsr env;
        env.prepare(1, kRate);

        // 実機のレジスタ幅。オペレータ側の prepare() と同じものを与える。
        // 省くと calcRegRate が regMax=0 で割り算し、LUT を範囲外参照して落ちる。
        env.setParamMax(
            CPV::OplRg::Ar::max,
            CPV::OplRg::Dr::max,
            CPV::OplRg::Sl::max,
            CPV::OplRg::Rr::max,
            CPV::OplRg::Tl::max
        );
        env.setParameters(makeParams(tl, ksl));
        return env;
    }

    float toDb(float gain) { return -20.0f * std::log10(gain); }
}

// OplAdsr : TL 1 段が 0.75dB の減衰になる
TEST_CASE("OplAdsr: one TL step is 0.75dB")
{
    // 実機の TL レジスタは 1 段 0.75dB。以前は TL を二重に掛けていた。
    auto a = make(0);
    auto b = make(8);

    float ga = a.noteOn(1.0f, 60);
    float gb = b.noteOn(1.0f, 60);

    CHECK(ga == doctest::Approx(1.0f));
    CHECK(toDb(gb) - toDb(ga) == doctest::Approx(8 * 0.75f).epsilon(0.01));
}

// OplAdsr : noteOn した音程で KSL が効く
TEST_CASE("OplAdsr: KSL uses the note passed to noteOn")
{
    // 以前は m_noteNumber の設定が KSL 計算より後ろにあり、
    // 1 つ前のノートの音程で計算されていた。
    auto env = make(0, 3);   // 6.0 dB/oct

    float low = env.noteOn(1.0f, 60);
    float high = env.noteOn(1.0f, 72);

    CHECK(toDb(high) - toDb(low) == doctest::Approx(6.0f).epsilon(0.01));
}

// OplAdsr : TL と KSL の合算は TL レジスタの全域で頭打ち
TEST_CASE("OplAdsr: TL plus KSL is capped at the TL register range")
{
    // 実機のレベルスケーリングは TL から引く形なので、
    // TL レジスタより深くは減衰しない。
    auto env = make(63, 3);

    float g = env.noteOn(1.0f, 127);

    CHECK(g > 0.0f);
    CHECK(toDb(g) <= 63 * 0.75f + 0.01f);
}

// OplAdsr : ベロシティが素直に掛かる
TEST_CASE("OplAdsr: velocity scales the level")
{
    auto env = make(0);

    CHECK(env.noteOn(1.0f, 60) == doctest::Approx(1.0f));
    CHECK(env.noteOn(0.5f, 60) == doctest::Approx(0.5f));
    CHECK(env.noteOn(0.25f, 60) == doctest::Approx(0.25f));
}

// OplAdsr : バイパス時はベロシティが素通しになる
TEST_CASE("OplAdsr: bypassed noteOn passes velocity through")
{
    OplAdsr env;
    env.prepare(1, kRate);
    env.setParamMax(CPV::OplRg::Ar::max, CPV::OplRg::Dr::max, CPV::OplRg::Sl::max,
                    CPV::OplRg::Rr::max, CPV::OplRg::Tl::max);

    auto p = makeParams(32);
    p.bypass = true;
    env.setParameters(p);

    CHECK(env.isBypass());
    CHECK(env.noteOn(0.7f, 60) == doctest::Approx(0.7f));
}

// OplAdsr : 出力が 0〜1 に収まり有限
TEST_CASE("OplAdsr: output stays finite within 0..1")
{
    auto env = make(0);

    float level = env.noteOn(1.0f, 60);

    for (int i = 0; i < 48000; ++i) {
        level = env.updateEnvelopeState(level);

        REQUIRE(std::isfinite(level));
        REQUIRE(level >= -1.0e-4f);
        REQUIRE(level <= 1.0f + 1.0e-4f);

        if (i == 24000) env.noteOff();
    }
}
