#pragma once

#include <cstdint>
#include <vector>

// --- PlayStation SPU (VAG) / CD-ROM XA の 4bit ADPCM ---
//
// どちらも「ブロックごとに shift と IIR フィルタを選ぶ」方式で、
// 復号式は共通。違いは 1 ブロックのサンプル数とフィルタの本数。
//
//   VAG : 16バイト = 28サンプル、フィルタ 5 種
//   XA  : 1サウンドユニット = 28サンプル、フィルタ 4 種
//         (XA は VAG のフィルタ表の先頭 4 つと同じ)
//
// 係数は 1/64 単位。prev1 * f0 + prev2 * f1 を予測値とする。
namespace PsxAdpcm {
    inline constexpr int samplesPerBlock = 28;
    inline constexpr int maxShift = 12;

    struct Filter {
        int f0;
        int f1;
    };

    inline const Filter filters[5] = {
        {   0,   0 },
        {  60,   0 },
        { 115, -52 },
        {  98, -55 },
        { 122, -60 },
    };

    inline constexpr int vagFilterCount = 5;
    inline constexpr int xaFilterCount = 4;
}

class PsxAdpcmCodec {
public:
    // filterCount に PsxAdpcm::vagFilterCount / xaFilterCount を渡して使い分ける
    static void process(std::vector<int16_t>& samples, int filterCount);

private:
    static double encodeBlock(
        const int16_t* src,
        int count,
        int shift,
        int filter,
        int prev1,
        int prev2,
        int16_t* dest
    );
};
