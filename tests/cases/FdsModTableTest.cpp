#include "doctest/doctest.h"

#include <cmath>

#include "Generator/Fds/GenFdsModTable.h"

// FdsMod : 位相 0 の変調量は 0
TEST_CASE("FdsMod: modulation is 0 at phase 0")
{
    // 発音の頭でピッチが飛ばないよう、出力は「テーブルを進める前」の
    // カウンタ値になっている。
    for (int t = 0; t < FdsMod::tableCount; ++t) {
        CHECK(FdsMod::value(t, 0.0f) == doctest::Approx(0.0f));
    }
}

// FdsMod : 作り置きテーブルの DC は 0
TEST_CASE("FdsMod: built-in tables have zero DC")
{
    // DC が乗ると変調をかけただけで音程がずれる。
    for (int t = 0; t < FdsMod::tableCount; ++t) {
        float dc = 0.0f;

        for (int i = 0; i < FdsMod::tableSize; ++i) {
            dc += FdsMod::steps[t][i];
        }

        CHECK(std::fabs(dc / (float)FdsMod::tableSize) < 0.02f);
    }
}

// FdsMod : 出力は -1.0〜1.0 に収まる
TEST_CASE("FdsMod: output stays within -1.0..1.0")
{
    for (int t = 0; t < FdsMod::tableCount; ++t) {
        for (int i = 0; i < FdsMod::tableSize; ++i) {
            CHECK(FdsMod::steps[t][i] >= -1.0f);
            CHECK(FdsMod::steps[t][i] <= 1.0f);
        }
    }
}

// FdsMod : makeSteps が作り置きと一致する
TEST_CASE("FdsMod: makeSteps reproduces the built-in tables")
{
    // ユーザー編集テーブルは makeSteps で積算するので、同じ関数が
    // 作り置きを再現できることを確かめておく。
    for (int t = 0; t < FdsMod::tableCount; ++t) {
        auto s = FdsMod::makeSteps(FdsMod::tables[t]);

        for (int i = 0; i < FdsMod::tableSize; ++i) {
            CHECK(s[i] == doctest::Approx(FdsMod::steps[t][i]));
        }
    }
}

// FdsMod : 3bit 値と増減量の対応が実機どおり
TEST_CASE("FdsMod: 3bit entries decode to the hardware increments")
{
    //  0 = +0 / 1 = +1 / 2 = +2 / 3 = +4
    //  4 = リセット (積算側で扱うので増減は 0)
    //  5 = -4 / 6 = -2 / 7 = -1
    CHECK(FdsMod::decodeIncrement(0) == 0);
    CHECK(FdsMod::decodeIncrement(1) == 1);
    CHECK(FdsMod::decodeIncrement(2) == 2);
    CHECK(FdsMod::decodeIncrement(3) == 4);
    CHECK(FdsMod::decodeIncrement(4) == 0);
    CHECK(FdsMod::decodeIncrement(5) == -4);
    CHECK(FdsMod::decodeIncrement(6) == -2);
    CHECK(FdsMod::decodeIncrement(7) == -1);
}

// FdsMod : リセットエントリで次のステップから 0 に戻る
TEST_CASE("FdsMod: reset entry clears the counter on the next step")
{
    std::array<int, FdsMod::tableSize> table{};

    for (int i = 0; i < FdsMod::tableSize; ++i) {
        table[i] = (i < 8) ? 3 : (i == 8 ? 4 : 0);   // +4 を 8 回 -> リセット -> 平坦
    }

    auto s = FdsMod::makeSteps(table);

    CHECK(s[8] > 0.9f);                     // リセット直前は積み上がっている
    CHECK(s[9] == doctest::Approx(0.0f));   // 次のステップで 0
    CHECK(s[20] == doctest::Approx(0.0f));
}

// FdsMod : カウンタは符号付き 7bit でラップする
TEST_CASE("FdsMod: counter wraps as signed 7bit")
{
    std::array<int, FdsMod::tableSize> table{};
    table.fill(3);   // +4 を 32 回 = +128 でちょうど一周

    auto s = FdsMod::makeSteps(table);

    float mn = 1.0e9f;

    for (int i = 0; i < FdsMod::tableSize; ++i) {
        mn = std::fmin(mn, s[i]);
    }

    CHECK(mn < -0.1f);   // 63 を越えて負側へ折り返している
}

// FdsMod : valueFromSteps が位相を 32 段へ丸める
TEST_CASE("FdsMod: valueFromSteps quantises phase to 32 steps")
{
    auto s = FdsMod::makeSteps(FdsMod::tables[0]);

    CHECK(FdsMod::valueFromSteps(s, 0.0f) == doctest::Approx(s[0]));
    CHECK(FdsMod::valueFromSteps(s, 0.5f) == doctest::Approx(s[16]));
    CHECK(FdsMod::valueFromSteps(s, 0.999f) == doctest::Approx(s[31]));
}
