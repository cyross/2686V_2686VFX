#include "doctest/doctest.h"

#include <vector>

#include "Effect/Fx/FxOrder.h"

// FX の並びのならし
//
// 効果の数はプラグインごとに違う (2686VFX だけ 1 つ多い)。以前は書かれていた
// 番号をそのまま流し込んでいたため、多いほうで書いた並びを少ないほうで読むと、
// ある効果が処理から抜け落ちて別の効果が二重に掛かっていた。

TEST_CASE("normalizeFxOrder: a full valid order is kept as written")
{
    const std::vector<int> in{ 3, 1, 0, 2 };

    CHECK(normalizeFxOrder(in, 4) == std::vector<int>{ 3, 1, 0, 2 });
}

TEST_CASE("normalizeFxOrder: out-of-range entries are dropped and the rest filled in")
{
    // 9 個の効果を持つプラグインで書いた並びを、8 個のプラグインで読む。
    // 8 は範囲外なので落ち、抜けたぶんは後ろへ足される。
    const std::vector<int> in{ 8, 0, 1, 2, 3, 4, 5, 6, 7 };

    const auto out = normalizeFxOrder(in, 8);

    CHECK(out == std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 });
}

TEST_CASE("normalizeFxOrder: duplicates keep only the first occurrence")
{
    const std::vector<int> in{ 2, 2, 0, 2 };

    const auto out = normalizeFxOrder(in, 4);

    CHECK(out == std::vector<int>{ 2, 0, 1, 3 });
}

TEST_CASE("normalizeFxOrder: missing entries are appended in order")
{
    const std::vector<int> in{ 3 };

    CHECK(normalizeFxOrder(in, 4) == std::vector<int>{ 3, 0, 1, 2 });
}

TEST_CASE("normalizeFxOrder: an empty order becomes the natural order")
{
    CHECK(normalizeFxOrder({}, 5) == std::vector<int>{ 0, 1, 2, 3, 4 });
}

TEST_CASE("normalizeFxOrder: negative entries are dropped")
{
    const std::vector<int> in{ -1, 1, -3, 0 };

    CHECK(normalizeFxOrder(in, 3) == std::vector<int>{ 1, 0, 2 });
}

// 出てきた並びは必ず 0..effectSize-1 が 1 回ずつ。ここが崩れると
// 効果の抜け落ちと二重掛けが起きる。
TEST_CASE("normalizeFxOrder: the result is always a permutation of every effect")
{
    const std::vector<std::vector<int>> inputs{
        {}, { 0 }, { 7, 7, 7 }, { 100, -5, 2 }, { 5, 4, 3, 2, 1, 0 }, { 0, 1, 2, 3, 4, 5, 6, 7, 8 }
    };

    for (int size : { 1, 4, 8, 9 })
    {
        for (const auto& in : inputs)
        {
            const auto out = normalizeFxOrder(in, size);

            REQUIRE(out.size() == (size_t)size);

            std::vector<int> seen((size_t)size, 0);

            for (int id : out)
            {
                REQUIRE(id >= 0);
                REQUIRE(id < size);

                seen[(size_t)id]++;
            }

            for (int n : seen) CHECK(n == 1);
        }
    }
}

TEST_CASE("normalizeFxOrder: a non-positive size yields nothing")
{
    CHECK(normalizeFxOrder({ 0, 1 }, 0).empty());
}
