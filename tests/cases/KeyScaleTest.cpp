#include "doctest/doctest.h"

#include <cmath>

#include "Effect/KeyScale/Opl/KSOpl.h"

namespace
{
    // ゲインを減衰 dB へ戻す
    float toDb(float gain) { return -20.0f * std::log10(gain); }

    KSOpl makeKs(int ksl)
    {
        KSOpl ks;
        KSOplParams p;
        p.ksr = false;
        p.ksl = ksl;
        ks.setParameters(p);
        return ks;
    }
}

// KSOpl : KSL レジスタの並びが実機どおり (01 と 10 が逆)
TEST_CASE("KSOpl: KSL register order matches the hardware")
{
    // 実機 YM3812 / YMF262 は 00=0dB / 01=3.0dB / 10=1.5dB / 11=6.0dB per oct。
    // 直感的な昇順ではないので、ここを取り違えると音色が変わる。
    // C4(60) を基準に 1 オクターブ上の C5(72) で減衰量を測る。
    const float expected[4] = { 0.0f, 3.0f, 1.5f, 6.0f };

    for (int ksl = 0; ksl < 4; ++ksl) {
        auto ks = makeKs(ksl);

        float base = ks.calcLevelScalingGain(60);
        float oct = ks.calcLevelScalingGain(72);

        float diffDb = toDb(oct) - toDb(base);

        CHECK(diffDb == doctest::Approx(expected[ksl]).epsilon(0.01));
    }
}

// KSOpl : KSL=0 なら音程によらず減衰しない
TEST_CASE("KSOpl: KSL=0 never attenuates")
{
    auto ks = makeKs(0);

    for (int note = 0; note < 128; ++note) {
        CHECK(ks.calcLevelScalingGain(note) == doctest::Approx(1.0f));
    }
}

// KSOpl : 基準より低い音では減衰しない
TEST_CASE("KSOpl: notes below the pivot are not attenuated")
{
    // octaveDiff が負のときは 0 に丸めているので、C3 以下は素通し。
    auto ks = makeKs(3);   // 6.0 dB/oct

    for (int note = 0; note <= 48; ++note) {
        CHECK(ks.calcLevelScalingGain(note) == doctest::Approx(1.0f));
    }

    CHECK(ks.calcLevelScalingGain(60) < 1.0f);
}

// KSOpl : 高い音ほど減衰が強い
TEST_CASE("KSOpl: attenuation grows with pitch")
{
    auto ks = makeKs(3);

    float prev = 2.0f;

    for (int note = 48; note < 128; ++note) {
        float g = ks.calcLevelScalingGain(note);

        CHECK(g <= prev + 1.0e-6f);
        CHECK(g > 0.0f);

        prev = g;
    }
}

// KSOpl : KSR は 0〜15 に収まる
TEST_CASE("KSOpl: KSR stays within 0..15")
{
    KSOpl ks;
    KSOplParams p;
    p.ksr = true;
    p.ksl = 0;
    ks.setParameters(p);

    for (int note = 0; note < 128; ++note) {
        int r = ks.calcKeyScaleRate(note);

        CHECK(r >= 0);
        CHECK(r <= 15);
    }
}
