#include <doctest/doctest.h>

#include <vector>

#include "Gui/Components/WavePreview/WavePreviewSource.h"
#include "Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h"
#include "Core/Synth/CommonParams.h"
#include "Generator/Fds/GenFdsModTable.h"
#include "Processor/Opzx7/ProcessorOpzx7Values.h"

// ============================================================================
// 波形プレビューの折れ線
// ============================================================================
// Sample & Hold は保持の長さが Shape ごとに違う。以前は Shape ごとに
// 「段が 8 つ見えるまで」窓を伸ばしていたため、4 でも 64 でも同じ見た目に
// なり違いが分からなかった。時間軸が揃っているかをここで確かめる。
namespace
{
    // 値が変わった位置を集める
    std::vector<int> changePoints(const std::vector<float>& v)
    {
        std::vector<int> out;

        for (int i = 1; i < (int)v.size(); ++i) {
            if (v[(size_t)i] != v[(size_t)(i - 1)]) out.push_back(i);
        }

        return out;
    }
}

TEST_CASE("Opzx7 LFO preview keeps a common time base across sample and hold shapes")
{
    // shape -> 保持する周期数。折れ線は 1 点が 1 周期にあたるので、
    // 値が変わってよいのはこの倍数の位置だけになる。
    const std::pair<int, int> table[] = {
        {  5,  1 },
        {  8,  4 },
        {  9,  8 },
        { 10, 16 },
        { 11, 32 },
        { 12, 64 },
    };

    for (const auto& [shape, holdCycles] : table) {
        CAPTURE(shape);
        CAPTURE(holdCycles);

        const auto v = WavePreviewSource::opzx7LfoPm(shape);

        REQUIRE(v.size() == (size_t)WavePreviewSource::points);

        const auto changes = changePoints(v);

        // 保持の途中で値が変わらない
        for (int at : changes) {
            CAPTURE(at);
            CHECK(at % holdCycles == 0);
        }

        // 段が並んで見えている
        CHECK(changes.size() > 0);
    }
}

TEST_CASE("Opzx7 LFO preview draws continuous shapes over a single period")
{
    // 段の無い波形は 1 周期を細かく描く。三角波の折り返しが 1 つだけ
    // 現れることで、1 周期ぶんが描かれていることを確かめる。
    const auto v = WavePreviewSource::opzx7LfoPm(4);

    REQUIRE(v.size() == (size_t)WavePreviewSource::points);

    int peaks = 0;

    for (int i = 1; i + 1 < (int)v.size(); ++i) {
        if (v[(size_t)i] > v[(size_t)(i - 1)] && v[(size_t)i] >= v[(size_t)(i + 1)]) ++peaks;
    }

    CHECK(peaks == 1);
}

TEST_CASE("SSG HW ENV preview keeps a common time base across sample and hold shapes")
{
    // shape -> 保持するサイクル数。折れ線は 1 点が 1 サイクルにあたる。
    //
    // Sample & Hold の 1 サイクルは位相 1.0 ぶんで、波形 1 周期の半分になる。
    const std::pair<SsgHwShape, int> table[] = {
        { SsgHwShape::SampleHold,    1 },
        { SsgHwShape::SampleHold4,   4 },
        { SsgHwShape::SampleHold8,   8 },
        { SsgHwShape::SampleHold16, 16 },
        { SsgHwShape::SampleHold32, 32 },
        { SsgHwShape::SampleHold64, 64 },
    };

    for (const auto& [shape, holdCycles] : table) {
        CAPTURE((int)shape);
        CAPTURE(holdCycles);

        // スムースを入れると値が毎サンプル動くので、段を見るときは切る
        const auto v = WavePreviewSource::ssgHwEnv((int)shape, 0.0f, 1.0f, false);

        REQUIRE(v.size() == (size_t)WavePreviewSource::points);

        const auto changes = changePoints(v);

        // 保持の途中で値が変わらない
        for (int at : changes) {
            CAPTURE(at);
            CHECK(at % holdCycles == 0);
        }

        // 段が並んで見えている
        CHECK(changes.size() > 0);
    }
}

namespace
{
    // 折れ線が上下に振れているか (横一直線でないか)
    bool hasSwing(const std::vector<float>& v)
    {
        for (float x : v) {
            if (x != v[0]) return true;
        }

        return false;
    }
}

TEST_CASE("WT MODULATION preview draws every shape")
{
    const std::array<float, 32> emptyWave = { 0.0f };
    const std::array<int, 32> emptyTable = { 0 };

    // HuC6280 と FdsUser は読み込んだデータが要る。それ以外は Shape だけで
    // 形が決まるので、何も渡さなくても振れていなければならない。
    for (int shape = 0; shape <= (int)WtModShape::WsSweepDown; ++shape) {
        CAPTURE(shape);

        const auto v = WavePreviewSource::wtMod(shape, emptyWave, emptyTable);

        REQUIRE(v.size() == (size_t)WavePreviewSource::points);
        CHECK(hasSwing(v));
    }
}

TEST_CASE("WT MODULATION preview follows the loaded wave and table")
{
    const std::array<float, 32> emptyWave = { 0.0f };
    const std::array<int, 32> emptyTable = { 0 };

    // ---- HuC6280 : 32 サンプルの波形メモリ ----
    // 何も読み込んでいなければ変調しないので横一直線でよい。
    CHECK_FALSE(hasSwing(WavePreviewSource::wtMod((int)WtModShape::HuC6280Wave, emptyWave, emptyTable)));

    std::array<float, 32> ramp = { 0.0f };

    for (int i = 0; i < 32; ++i) ramp[(size_t)i] = (float)i / 31.0f * 2.0f - 1.0f;

    CHECK(hasSwing(WavePreviewSource::wtMod((int)WtModShape::HuC6280Wave, ramp, emptyTable)));

    // ---- FdsUser : 32 エントリの増減テーブル ----
    CHECK_FALSE(hasSwing(WavePreviewSource::wtMod((int)WtModShape::FdsUser, emptyWave, emptyTable)));

    // 作り置きのテーブルを入れれば階段が出る
    CHECK(hasSwing(WavePreviewSource::wtMod((int)WtModShape::FdsUser, emptyWave, FdsMod::tables[0])));
}

TEST_CASE("Opzx7 WS preview draws every waveform")
{
    // 波形メモリ / PCM は読み込んでいない状態
    std::vector<float> empty;

    // 73 種すべてを一巡させる。番号を足したときに落ちないよう上限も見る。
    constexpr int wsCount = 73;

    for (int ws = 0; ws < wsCount; ++ws) {
        CAPTURE(ws);

        const auto v = WavePreviewSource::opzx7Ws(ws, empty, empty, empty);

        REQUIRE(v.size() == (size_t)WavePreviewSource::points);

        // どの波形も 1 周ぶんの中で必ず動く
        CHECK(hasSwing(v));

        for (float x : v) {
            CHECK(std::isfinite(x));
        }
    }
}

TEST_CASE("Opzx7 WS preview follows the loaded wave memory")
{
    std::vector<float> empty;

    // 読み込んでいなければ音源側と同じくサイン波が返る
    const auto sine = WavePreviewSource::opzx7Ws(0, empty, empty, empty);
    const auto emptyWt = WavePreviewSource::opzx7Ws(Opzx7PrValue::wtIndex, empty, empty, empty);

    CHECK(emptyWt == sine);

    // 読み込んでいればその中身になる
    std::vector<float> square(64, -1.0f);

    for (int i = 0; i < 32; ++i) square[(size_t)i] = 1.0f;

    const auto loadedWt = WavePreviewSource::opzx7Ws(Opzx7PrValue::wtIndex, square, empty, empty);

    CHECK(loadedWt != sine);
    CHECK(hasSwing(loadedWt));

    // WT2 と PCM も同じ扱い
    CHECK(WavePreviewSource::opzx7Ws(Opzx7PrValue::wt2Index, empty, empty, empty) == sine);
    CHECK(WavePreviewSource::opzx7Ws(Opzx7PrValue::wt2Index, empty, square, empty) != sine);
    CHECK(WavePreviewSource::opzx7Ws(Opzx7PrValue::pcmIndex, empty, empty, empty) == sine);
    CHECK(WavePreviewSource::opzx7Ws(Opzx7PrValue::pcmIndex, empty, empty, square) != sine);
}

TEST_CASE("Audio file preview cuts out the range that actually plays")
{
    // 1 秒 (48000 サンプル) の目印つきデータ。位置が分かるよう値を並べる。
    constexpr double rate = 48000.0;

    std::vector<float> data((size_t)rate, 0.0f);

    for (size_t i = 0; i < data.size(); ++i) data[i] = (float)i / (float)data.size() * 2.0f - 1.0f;

    SUBCASE("全体")
    {
        auto env = WavePreviewSource::audioFile(data, rate, 0.0f, 1.0f);

        REQUIRE(env.mins.size() == (size_t)WavePreviewSource::points);
        REQUIRE(env.maxs.size() == env.mins.size());

        // 先頭は -1 付近、末尾は +1 付近
        CHECK(env.mins.front() == doctest::Approx(-1.0f).epsilon(0.05));
        CHECK(env.maxs.back() == doctest::Approx(1.0f).epsilon(0.05));
    }

    SUBCASE("オフセットで頭を飛ばす")
    {
        // 500ms 進めると、ちょうど真ん中 (値 0.0) から始まる
        auto env = WavePreviewSource::audioFile(data, rate, 500.0f, 1.0f);

        CHECK(env.mins.front() == doctest::Approx(0.0f).epsilon(0.05));
        CHECK(env.maxs.back() == doctest::Approx(1.0f).epsilon(0.05));
    }

    SUBCASE("比率で尻を詰める")
    {
        // 半分だけ鳴らすと、末尾はちょうど真ん中の値になる
        auto env = WavePreviewSource::audioFile(data, rate, 0.0f, 0.5f);

        CHECK(env.mins.front() == doctest::Approx(-1.0f).epsilon(0.05));
        CHECK(env.maxs.back() == doctest::Approx(0.0f).epsilon(0.05));
    }

    SUBCASE("上下幅を潰さない")
    {
        // 1 区間に何サンプル入っても、その中の最小と最大を拾う。
        // 単純な間引きだと交互に並ぶ波形が消えてしまう。
        std::vector<float> alternating(4096, 0.0f);

        for (size_t i = 0; i < alternating.size(); ++i) alternating[i] = (i % 2 == 0) ? 1.0f : -1.0f;

        auto env = WavePreviewSource::audioFile(alternating, rate, 0.0f, 1.0f);

        for (size_t i = 0; i < env.mins.size(); ++i) {
            CAPTURE(i);
            CHECK(env.mins[i] == doctest::Approx(-1.0f));
            CHECK(env.maxs[i] == doctest::Approx(1.0f));
        }
    }

    SUBCASE("空なら何も返さない")
    {
        std::vector<float> nothing;

        auto env = WavePreviewSource::audioFile(nothing, rate, 0.0f, 1.0f);

        CHECK(env.mins.empty());
        CHECK(env.maxs.empty());
    }
}
