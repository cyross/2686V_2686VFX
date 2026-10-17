#include "doctest/doctest.h"

#include <cmath>

#include "Shared/Generator/Pulse/GenPulse2A03.h"

TEST_CASE("2A03: デューティ比は実機の 4 種類のうち最も近いものへ寄る")
{
    CHECK(Pulse2A03::nearestDuty(0.5f) == 2);
    CHECK(Pulse2A03::nearestDuty(0.4375f) == 2);
    CHECK(Pulse2A03::nearestDuty(0.3125f) == 1);
    CHECK(Pulse2A03::nearestDuty(0.0625f) == 0);
    CHECK(Pulse2A03::nearestDuty(0.8f) == 3);
}

TEST_CASE("2A03: 8 段の並びで、高い側の割合がデューティ比になる")
{
    for (int d = 0; d < 4; ++d) {
        int high = 0;

        for (int s = 0; s < 8; ++s) high += (int)Pulse2A03::sample(((float)s + 0.5f) / 8.0f, d);

        CHECK(high == (int)std::lround(Pulse2A03::duties[(size_t)d] * 8.0f));
    }
}

TEST_CASE("2A03: 周波数は 11bit タイマーの刻みへ寄り、t < 8 は鳴らない")
{
    // A4 (440Hz) は t = 253 で 440.4Hz
    CHECK(Pulse2A03::quantizeFrequency(440.0) == doctest::Approx(1789772.727 / (16.0 * 254.0)));

    // t = 7 になる高さは鳴らない
    CHECK(Pulse2A03::quantizeFrequency(1789772.727 / (16.0 * 8.0)) == 0.0);

    // 低すぎる音はタイマーの上限で鳴らす
    CHECK(Pulse2A03::quantizeFrequency(10.0) == doctest::Approx(1789772.727 / (16.0 * 2048.0)));
}

TEST_CASE("2A03: 音量は 16 段で、最大が 1 になる")
{
    CHECK(Pulse2A03::volume(1.0f) == doctest::Approx(1.0f));
    CHECK(Pulse2A03::volume(0.0f) == 0.0f);
    CHECK(Pulse2A03::volume(0.01f) == 0.0f);

    // 非線形なので、半分の音量は半分より大きい
    CHECK(Pulse2A03::volume(0.5f) > 0.5f);
}

TEST_CASE("2A03: 出力のフィルタは直流を抜く")
{
    for (auto model : { Pulse2A03::OutputFilter::Model::Fc, Pulse2A03::OutputFilter::Model::Nes }) {
        Pulse2A03::OutputFilter f;
        f.prepare(44100.0, model);

        float y = 0.0f;

        for (int i = 0; i < 44100; ++i) y = f.process(1.0f);

        CHECK(std::abs(y) < 0.01f);
    }
}
