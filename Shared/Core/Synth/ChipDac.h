#pragma once

#include <array>
#include <cmath>
#include <cstdint>

// ============================================================================
// CHIP DAC (実機の DAC を通す)
// ============================================================================
// QUALITY の「CHIP DAC」で使う。既定は切れていて、切っているあいだは
// これまでと同じ音で鳴る。
//
// FM 音源は、全チャンネルを足した 16bit の値を DAC へ送る。DAC の形は
// 音源ごとに違う。値は YM3012 のデータシートと ymfm (MAME の FM 音源の核)
// による。
//
//   OPN  (YM2203)  YM3014B  浮動小数点 (仮数 10bit + 指数 3bit)
//   OPM  (YM2151)  YM3012   浮動小数点 (同上)
//   OPL  (YM3526)  YM3014B  浮動小数点 (同上)
//   OPNA (YM2608)  16bit のまま (ymfm は浮動小数点へ丸めない)
//   OPL3 (YMF262)  YAC512   16bit のまま
//
// 浮動小数点の DAC は、大きな値ほど下の桁が落ちる。仮数の 10bit に収まる
// まで右へずらし、ずらした桁を捨てる (指数の最小は 1。ずらさない値は
// 指数 0 になるが、データシートで「使えない」とされている)。
//
// どこまでが 1 チャンネルの最大かも音源で違う。2686V の 1 キャリアの
// 最大 (ベロシティ 1・LEVEL 1 で 0.5) を、実機の 1 チャンネルの最大へ
// 合わせる。OPN・OPM・OPL3 は 14bit (±8192)、OPNA・OPL は 13bit (±4096)。
// そこから 16bit を超えたぶんは実機と同じく頭打ちになる。
namespace ChipDac
{
    enum class Format
    {
        Float10_3, // 仮数 10bit + 指数 3bit
        Linear16,  // 16bit のまま
    };

    struct Spec
    {
        Format format;

        // 2686V の値 1.0 が、実機の 16bit の値でいくつに当たるか
        float scale;
    };

    // 1 キャリアの最大 (0.5) が 1 チャンネルの最大になる倍率
    inline constexpr float scale14 = 8192.0f / 0.5f;
    inline constexpr float scale13 = 4096.0f / 0.5f;

    inline constexpr Spec opn{ Format::Float10_3, scale14 };
    inline constexpr Spec opm{ Format::Float10_3, scale14 };
    inline constexpr Spec opl{ Format::Float10_3, scale13 };
    inline constexpr Spec opna{ Format::Linear16, scale13 };
    inline constexpr Spec opl3{ Format::Linear16, scale14 };

    // 16bit の値を浮動小数点の DAC へ通して戻す (ymfm の roundtrip_fp と同じ)
    inline int32_t roundtripFloat(int32_t value)
    {
        if (value < -32768) return -32768;
        if (value > 32767) return 32767;

        // 符号の桁から下へ、符号と同じ桁がいくつ続くか。負の数は反転して数える。
        const int32_t scan = value ^ (value >> 31);

        // 14bit 目から下へ見て、最初に立っている桁の位置が指数になる
        int exponent = 1;

        for (int bit = 14; bit >= 9; --bit) {
            if ((scan >> bit) & 1) { exponent = bit - 7; break; }
        }

        // 指数 1 はずらさない。2 以上はずらしたぶんの桁を捨てる。
        const int32_t mask = (1 << (exponent - 1)) - 1;

        return value & ~mask;
    }

    inline float process(float x, const Spec& spec)
    {
        const double scaled = std::round((double)x * (double)spec.scale);
        const int32_t v = scaled < -32768.0 ? -32768 : (scaled > 32767.0 ? 32767 : (int32_t)scaled);

        const int32_t out = spec.format == Format::Float10_3 ? roundtripFloat(v) : v;

        return (float)out / spec.scale;
    }

    inline void process(float* data, int numSamples, const Spec& spec)
    {
        for (int i = 0; i < numSamples; ++i) data[i] = process(data[i], spec);
    }
}

// ============================================================================
// SSG の音量の DAC
// ============================================================================
// SSG (YM2149 互換。OPN・OPNA の SSG もこれ) は、チャンネルごとに対数の
// DAC を持つ。音量のレジスタは 4bit (16 段)、ハードウェアエンベロープは
// 5bit (32 段) で、どちらも同じ 32 段の表を引く。レジスタの値 v は表の
// 2v + 1 番目に当たる。
//
// 表は ymfm (MAME の実測をもとに、0 が 0 になるようずらしたもの) による。
namespace SsgDac
{
    inline constexpr std::array<int, 32> amplitudes = {
            0,    32,    78,   141,   178,   222,   262,   306,
          369,   441,   509,   585,   701,   836,   965,  1112,
         1334,  1595,  1853,  2146,  2576,  3081,  3576,  4135,
         5000,  6006,  7023,  8155,  9963, 11976, 14132, 16382,
    };

    inline float step(int index)
    {
        return (float)amplitudes[(size_t)index] / (float)amplitudes[31];
    }

    // 0〜1 を受け取り、0〜1 で返す。1 を超える値は超えたぶんを比のまま残す。
    inline float apply(float level, int steps)
    {
        if (level <= 0.0f) return 0.0f;

        const float clipped = level > 1.0f ? 1.0f : level;
        const int v = (int)std::lround(clipped * (float)(steps - 1));

        if (v <= 0) return 0.0f;

        const float out = steps == 32 ? step(v) : step(v * 2 + 1);

        return out * (level / clipped);
    }

    // 音量のレジスタ (16 段)。エンベロープの値をレジスタの値と見なす。
    inline float volume(float level) { return apply(level, 16); }

    // ハードウェアエンベロープ (32 段)
    inline float envelope(float level) { return apply(level, 32); }
}
