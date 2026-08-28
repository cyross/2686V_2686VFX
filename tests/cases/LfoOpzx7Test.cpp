#include <doctest/doctest.h>

#include <vector>

#include "Effect/Lfo/Opzx7/LfoOpzx7Unit.h"

// ============================================================================
// OPZX7 LFO の波形
// ============================================================================
// Sample & Hold は「何周期ごとに値を引き直すか」で性格が決まる。以前は
// サイクル数ではなくサンプル数を数えていたため 8〜12 がすべてノイズと同じ
// 挙動になっていた。ここではその保持の長さを直接測る。
namespace
{
    // 1 周期をちょうど samplesPerPeriod サンプルにする組み合わせ。
    // 位相の増分が 2 のべき乗になるので、周期の区切りが誤差でぶれない。
    constexpr int samplesPerPeriod = 32;
    constexpr float freq = 8.0f;
    constexpr double rate = (double)freq * samplesPerPeriod;

    // Sync Delay = 1 は「遅延なしで位相を頭出し」
    constexpr int syncDelay = 1;

    // 周期ごとに 1 点ずつ拾う。
    //
    // 位相は加算してから折り返しを見るので、周期の最後の 1 サンプルは
    // すでに次の周期として扱われる。境界を踏まないよう周期の中ほどで拾う。
    std::vector<float> samplePerCycle(int shapeIndex, int cycles)
    {
        Opzx7LfoCoreUnit unit;

        unit.prepare(rate);
        unit.setParameters(syncDelay, true, freq, shapeIndex, 1.0f, 1.0f, 1.0f);
        unit.noteOn();

        constexpr int offset = samplesPerPeriod / 2;

        std::vector<float> out;
        out.reserve((size_t)cycles);

        for (int i = 0; i < cycles * samplesPerPeriod; ++i) {
            float v = unit.getSamplePm();

            if ((i % samplesPerPeriod) == offset) out.push_back(v);
        }

        return out;
    }

    std::vector<float> runSamples(int shapeIndex, int cycles)
    {
        Opzx7LfoCoreUnit unit;

        unit.prepare(rate);
        unit.setParameters(syncDelay, true, freq, shapeIndex, 1.0f, 1.0f, 1.0f);
        unit.noteOn();

        std::vector<float> out;
        out.reserve((size_t)cycles * samplesPerPeriod);

        for (int i = 0; i < cycles * samplesPerPeriod; ++i) out.push_back(unit.getSamplePm());

        return out;
    }
}

TEST_CASE("Opzx7 LFO sample and hold keeps its value for the specified cycles")
{
    // shape -> 保持する周期数
    const std::pair<int, int> table[] = {
        {  5,  1 },
        {  8,  4 },
        {  9,  8 },
        { 10, 16 },
        { 11, 32 },
        { 12, 64 },
    };

    // 保持の区画をこの数だけ並べて見る。
    //
    // ノイズ源の LFSR は全ビット 1 から始まるので、最初の十数ステップは
    // 出力ビットが 1 のまま続く。値が変わるところまで見るには、区画を
    // それより多く並べる必要がある。
    constexpr int blocks = 64;

    for (const auto& [shape, holdCycles] : table) {
        CAPTURE(shape);
        CAPTURE(holdCycles);

        const auto v = samplePerCycle(shape, holdCycles * blocks);

        // 同じ区画の中では値が変わらない
        for (int c = 0; c < (int)v.size(); ++c) {
            CAPTURE(c);
            CHECK(v[(size_t)c] == v[(size_t)(c - c % holdCycles)]);
        }

        // 区画をまたげば値が変わる。変わらなければ保持ではなく固定。
        bool changed = false;

        for (int b = 1; b < blocks; ++b) {
            if (v[(size_t)(b * holdCycles)] != v[0]) changed = true;
        }

        CHECK(changed);
    }
}

TEST_CASE("Opzx7 LFO one shot triangle is not overwritten by sample and hold")
{
    // Shape 7 は break が無く Shape 8 へ落ちていたため、三角波が
    // ホールド値で潰れていた。
    const auto v = runSamples(7, 4);

    // 1 周期目は三角波として動く
    bool hasSlope = false;

    for (int i = 1; i < samplesPerPeriod - 1; ++i) {
        if (v[(size_t)i] != v[(size_t)(i - 1)]) hasSlope = true;
    }

    CHECK(hasSlope);

    // ワンショットなので 2 周期目以降は無音
    for (int i = samplesPerPeriod; i < (int)v.size(); ++i) {
        CAPTURE(i);
        CHECK(v[(size_t)i] == 0.0f);
    }
}
