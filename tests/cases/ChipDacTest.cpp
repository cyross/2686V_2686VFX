#include "doctest/doctest.h"

#include <cmath>
#include <cstdint>

#include "Shared/Core/Synth/ChipDac.h"

namespace
{
    // ymfm の roundtrip_fp をそのまま書き写したもの。突き合わせに使う。
    int32_t ymfmRoundtrip(int32_t value)
    {
        if (value < -32768) return -32768;
        if (value > 32767) return 32767;

        const uint32_t scan = (uint32_t)(value ^ (value >> 31)) << 17;

        int clz = 0;

        for (int bit = 31; bit >= 0 && ((scan >> bit) & 1) == 0; --bit) ++clz;

        int exponent = 7 - clz;

        if (exponent < 1) exponent = 1;

        exponent -= 1;

        return value & ~((1 << exponent) - 1);
    }
}

TEST_CASE("CHIP DAC: 浮動小数点の DAC は ymfm と同じ値を返す")
{
    for (int32_t v = -40000; v <= 40000; ++v) {
        REQUIRE(ChipDac::roundtripFloat(v) == ymfmRoundtrip(v));
    }
}

TEST_CASE("CHIP DAC: 仮数に収まる小さな値はそのまま、大きな値ほど下の桁が落ちる")
{
    CHECK(ChipDac::roundtripFloat(511) == 511);
    CHECK(ChipDac::roundtripFloat(-512) == -512);

    // 1024〜2047 は 1 桁ずれる
    CHECK(ChipDac::roundtripFloat(1025) == 1024);

    // 16384〜32767 は 6 桁ずれる
    CHECK(ChipDac::roundtripFloat(16384 + 63) == 16384);
    CHECK(ChipDac::roundtripFloat(32767) == 32767 - 63);
}

TEST_CASE("CHIP DAC: 16bit を超えたぶんは頭打ちになる")
{
    // OPN の 1 キャリアの最大 (0.5) は 8192。4 本ぶんで 16bit に届く
    CHECK(ChipDac::process(0.5f, ChipDac::opn) == doctest::Approx(0.5f));
    // 頭打ちの 32767 も浮動小数点の DAC を通るので、下の 6 桁が落ちる
    CHECK(ChipDac::process(10.0f, ChipDac::opn) == doctest::Approx(32704.0f / ChipDac::scale14));
    CHECK(ChipDac::process(-10.0f, ChipDac::opna) == doctest::Approx(-32768.0f / ChipDac::scale13));
}

TEST_CASE("CHIP DAC: 16bit のままの DAC は 16bit の刻みへ寄るだけ")
{
    const float x = 0.123456f;
    const float y = ChipDac::process(x, ChipDac::opl3);

    CHECK(std::abs(y - x) <= 0.5f / ChipDac::scale14);
}

TEST_CASE("SSG DAC: 最大は 1、0 は 0、途中は対数で小さくなる")
{
    CHECK(SsgDac::volume(1.0f) == doctest::Approx(1.0f));
    CHECK(SsgDac::envelope(1.0f) == doctest::Approx(1.0f));
    CHECK(SsgDac::volume(0.0f) == 0.0f);
    CHECK(SsgDac::envelope(0.0f) == 0.0f);

    // レジスタの 14 は表の 29 番目
    CHECK(SsgDac::volume(14.0f / 15.0f) == doctest::Approx(11976.0f / 16382.0f));

    // 半分の音量はおよそ -20dB
    CHECK(SsgDac::volume(0.5f) < 0.15f);

    // 1 を超える指定は比のまま残す
    CHECK(SsgDac::volume(2.0f) == doctest::Approx(2.0f));
}
