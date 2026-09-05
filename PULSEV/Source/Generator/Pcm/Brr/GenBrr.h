#pragma once

#include <cstdint>
#include <vector>

// --- SNES / SPC700 (S-DSP) BRR (Bit Rate Reduction) ---
//
// 9バイト = 16サンプルのブロック単位で圧縮する。
//   ヘッダ 1バイト : shift(4bit) | filter(2bit) | loop(1bit) | end(1bit)
//   データ 8バイト : 4bit × 16
//
// 予測に IIR フィルタを 4 種持つのが特徴で、YM2608 系の
// 「ステップ幅を適応させる」方式とは根本的に考え方が違う。
namespace SnesBrr {
    inline constexpr int samplesPerBlock = 16;

    // shift は 0〜12 が通常。13〜15 は実機では特殊動作になるため使わない。
    inline constexpr int maxShift = 12;
    inline constexpr int filterCount = 4;
}

class BrrCodec {
public:
    // int16 列をブロック単位でエンコードし、デコード結果で上書きする
    static void process(std::vector<int16_t>& samples);

private:
    // 1 ブロック分を指定の shift / filter で符号化し、復号結果と誤差を返す
    static double encodeBlock(
        const int16_t* src,
        int count,
        int shift,
        int filter,
        int prev1,
        int prev2,
        int16_t* dest
    );

    // filter に応じた予測値を返す
    static int predict(int filter, int prev1, int prev2);
};
