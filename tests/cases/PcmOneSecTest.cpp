#include "doctest/doctest.h"

#include "Shared/Core/Synth/PcmOneSec.h"

// ============================================================================
// PCM の 1 秒モード (4.5.0)
// ============================================================================
// 区間を 1 秒の素材にまとめるときの、仮の位置から元の素材の位置への移し方を見る。
// 長さは素材の標本の数。ここでは 1 秒 = 1000 標本として数える。
namespace
{
    constexpr double kOneSec = 1000.0;

    namespace L = CPV::Pcm::OneSecLong;
    namespace S = CPV::Pcm::OneSecShort;

    double at(const PcmOneSec::Fit& f, double v, double playSize, bool& silent)
    {
        return PcmOneSec::map(f, v, playSize, silent);
    }
}

TEST_CASE("PcmOneSec: 長い区間の間引きは、区間全体を 1 秒へ縮める")
{
    const auto f = PcmOneSec::fit(4000.0, kOneSec, L::decimate, S::stretch);
    bool silent = true;

    CHECK(f.scale == doctest::Approx(4.0));
    CHECK(at(f, 0.0, 4000.0, silent) == doctest::Approx(0.0));
    CHECK(at(f, 500.0, 4000.0, silent) == doctest::Approx(2000.0));
    CHECK(at(f, 999.0, 4000.0, silent) == doctest::Approx(3996.0));
    CHECK_FALSE(silent);
}

TEST_CASE("PcmOneSec: 長い区間のスライスは、頭から 1 秒だけを素材の速さのまま使う")
{
    const auto f = PcmOneSec::fit(4000.0, kOneSec, L::slice, S::stretch);
    bool silent = true;

    CHECK(f.scale == doctest::Approx(1.0));
    CHECK(at(f, 500.0, 4000.0, silent) == doctest::Approx(500.0));
    CHECK(at(f, 999.0, 4000.0, silent) == doctest::Approx(999.0));
    CHECK_FALSE(silent);
}

TEST_CASE("PcmOneSec: 短い区間の拡張は、区間全体を 1 秒へ引き伸ばす")
{
    const auto f = PcmOneSec::fit(250.0, kOneSec, L::decimate, S::stretch);
    bool silent = true;

    CHECK(f.scale == doctest::Approx(0.25));
    CHECK(at(f, 500.0, 250.0, silent) == doctest::Approx(125.0));
    CHECK(at(f, 999.0, 250.0, silent) == doctest::Approx(249.75));
    CHECK_FALSE(silent);
}

TEST_CASE("PcmOneSec: 短い区間の繰り返しは、区間の終わりで頭へ戻る")
{
    const auto f = PcmOneSec::fit(250.0, kOneSec, L::decimate, S::repeat);
    bool silent = true;

    CHECK(f.repeat);
    CHECK(at(f, 100.0, 250.0, silent) == doctest::Approx(100.0));
    CHECK(at(f, 260.0, 250.0, silent) == doctest::Approx(10.0));
    CHECK(at(f, 760.0, 250.0, silent) == doctest::Approx(10.0));
    CHECK_FALSE(silent);
}

TEST_CASE("PcmOneSec: 短い区間の無音は、区間の終わりから 1 秒までを鳴らさない")
{
    const auto f = PcmOneSec::fit(250.0, kOneSec, L::decimate, S::silence);
    bool silent = true;

    CHECK(f.silence);
    CHECK(at(f, 100.0, 250.0, silent) == doctest::Approx(100.0));
    CHECK_FALSE(silent);

    at(f, 250.0, 250.0, silent);
    CHECK(silent);

    at(f, 999.0, 250.0, silent);
    CHECK(silent);
}

TEST_CASE("PcmOneSec: ちょうど 1 秒なら、まとめ方によらずそのまま")
{
    for (int l : { L::decimate, L::slice }) {
        for (int s : { S::stretch, S::repeat, S::silence }) {
            const auto f = PcmOneSec::fit(kOneSec, kOneSec, l, s);
            bool silent = true;

            CHECK(f.scale == doctest::Approx(1.0));
            CHECK_FALSE(f.repeat);
            CHECK_FALSE(f.silence);
            CHECK(at(f, 700.0, kOneSec, silent) == doctest::Approx(700.0));
            CHECK_FALSE(silent);
        }
    }
}
