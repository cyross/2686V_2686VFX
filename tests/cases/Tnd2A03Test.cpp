// ============================================================================
// FC (2A03) の三角波・ノイズと、SSG の出力 (4.3.0)
// ============================================================================
#include "doctest/doctest.h"

#include <cmath>
#include <set>

#include "Shared/Generator/Pulse/GenTnd2A03.h"

TEST_CASE("2A03 三角波: 4bit × 32 段で、15 … 0, 0 … 15 と並ぶ")
{
    int levels[32];

    for (int i = 0; i < 32; ++i) levels[i] = Tnd2A03::triangleLevel(((float)i + 0.5f) / 32.0f);

    CHECK(levels[0] == 15);
    CHECK(levels[1] == 14);
    CHECK(levels[15] == 0);
    CHECK(levels[16] == 0);
    CHECK(levels[17] == 1);
    CHECK(levels[31] == 15);

    // 0 と 15 が 2 段ずつ、ほかは 1 段ずつ下りて上る
    for (int i = 1; i < 16; ++i) CHECK(levels[i] == levels[i - 1] - 1);
    for (int i = 17; i < 32; ++i) CHECK(levels[i] == levels[i - 1] + 1);
}

TEST_CASE("2A03 三角波: DAC は 15 で 1、0 で 0、途中は曲がる")
{
    CHECK(Tnd2A03::triangleDac(15) == doctest::Approx(1.0f));
    CHECK(Tnd2A03::triangleDac(0) == doctest::Approx(0.0f));

    // 直線より上へふくらむ (小さい値ほど比が大きい)
    CHECK(Tnd2A03::triangleDac(8) > 8.0f / 15.0f);
    CHECK(Tnd2A03::triangleDac(8) < 1.0f);
}

TEST_CASE("2A03 三角波: 周波数はタイマーの刻みへ寄り、矩形波より 1 オクターブ低い")
{
    // A4 (440Hz) は t = 126 → 1789772.727 / (32 × 127)
    CHECK(Tnd2A03::quantizeTriangleFrequency(440.0) == doctest::Approx(Pulse2A03::cpuClock / (32.0 * 127.0)));

    // 同じタイマーでは、矩形波の半分の周波数になる
    CHECK(Pulse2A03::cpuClock / (32.0 * 127.0) == doctest::Approx(Pulse2A03::cpuClock / (16.0 * 127.0) / 2.0));

    // t < 2 は鳴らさない。低すぎる音はタイマーの上限で鳴らす
    CHECK(Tnd2A03::quantizeTriangleFrequency(40000.0) == doctest::Approx(0.0));
    CHECK(Tnd2A03::quantizeTriangleFrequency(1.0) == doctest::Approx(Pulse2A03::cpuClock / (32.0 * 2048.0)));
}

TEST_CASE("2A03 ノイズ: 15bit LFSR の長周期は 32,767 段で一周する")
{
    unsigned int lfsr = Tnd2A03::noiseInitial;
    std::set<unsigned int> seen;

    int period = 0;

    do {
        seen.insert(lfsr);
        Tnd2A03::stepNoise(lfsr);
        ++period;
    } while (lfsr != Tnd2A03::noiseInitial && period <= 40000);

    CHECK(period == Tnd2A03::noisePeriodLong);
    CHECK((int)seen.size() == Tnd2A03::noisePeriodLong);
}

TEST_CASE("2A03 ノイズ: ビット 0 が 0 のときに鳴る")
{
    unsigned int lfsr = 0b10u;   // 進めるとビット 0 が 1 になる

    CHECK(Tnd2A03::stepNoise(lfsr) == doctest::Approx(0.0f));

    lfsr = 0b100u;               // 進めるとビット 0 が 0 になる

    CHECK(Tnd2A03::stepNoise(lfsr) == doctest::Approx(1.0f));
}

TEST_CASE("2A03 ノイズ: 速さは 16 段の表のうち、比で最も近いものへ寄る")
{
    // 表の値そのものは動かない
    CHECK(Tnd2A03::nearestNoiseRate(Pulse2A03::cpuClock / 202.0) == doctest::Approx(Pulse2A03::cpuClock / 202.0));

    // 速すぎる・遅すぎる指定は端へ
    CHECK(Tnd2A03::nearestNoiseRate(1.0e7) == doctest::Approx(Pulse2A03::cpuClock / 4.0));
    CHECK(Tnd2A03::nearestNoiseRate(1.0) == doctest::Approx(Pulse2A03::cpuClock / 4068.0));
}

TEST_CASE("2A03 ノイズ: SSG の N.FREQ の範囲で、16 段すべてに届く")
{
    constexpr double knobMax = 30000.0;

    std::set<double> reached;

    for (int hz = 0; hz <= (int)knobMax; hz += 5) reached.insert(Tnd2A03::noiseRateFromSsg((double)hz, knobMax));

    reached.erase(0.0);

    CHECK(reached.size() == Tnd2A03::noisePeriods.size());

    // いちばん上はいちばん速い段
    CHECK(Tnd2A03::noiseRateFromSsg(knobMax, knobMax) == doctest::Approx(Pulse2A03::cpuClock / 4.0));
}

TEST_CASE("SSG: トーンとノイズは論理積で混ざる")
{
    // MIX 0 はトーンだけ、1 はノイズだけ
    CHECK(SsgReal::mix(1.0f, 0.0f, 0.0f) == doctest::Approx(1.0f));
    CHECK(SsgReal::mix(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0f));

    // 0.5 は AND。どちらかが 0 なら 0
    CHECK(SsgReal::mix(1.0f, 1.0f, 0.5f) == doctest::Approx(1.0f));
    CHECK(SsgReal::mix(1.0f, 0.0f, 0.5f) == doctest::Approx(0.0f));
    CHECK(SsgReal::mix(0.0f, 1.0f, 0.5f) == doctest::Approx(0.0f));
}

TEST_CASE("SSG: 直流カットは単極の矩形波を 0 中心へ戻し、平らなところが傾く")
{
    constexpr double rate = 48000.0;

    SsgReal::DcBlocker dc;
    dc.prepare(rate);

    // 100Hz の 0 / 1 の矩形波を 1 秒流す
    double sum = 0.0;
    float firstHigh = 0.0f, lastHigh = 0.0f;

    const int period = (int)(rate / 100.0);

    for (int i = 0; i < (int)rate; ++i) {
        const int pos = i % period;
        const float x = pos < period / 2 ? 1.0f : 0.0f;
        const float y = dc.process(x);

        if (i >= (int)rate - period) {
            sum += y;

            if (pos == 1) firstHigh = y;
            if (pos == period / 2 - 1) lastHigh = y;
        }
    }

    // 平均はほぼ 0 (直流が抜けている)
    CHECK(std::abs(sum / (double)period) < 0.02);

    // High の頭より終わりのほうが低い (サグ)
    CHECK(lastHigh < firstHigh);
}
