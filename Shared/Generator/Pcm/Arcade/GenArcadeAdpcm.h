#pragma once

#include <cstdint>
#include <vector>

// =====================================================================
// アーケード系チップの 4bit ADPCM
//
// !! 注意 !!
// 以下 3 つのテーブルと係数は、公開されているエミュレータ実装の挙動を
// もとにしたものです。BRR / VAG / XA / IMA と違って一次資料での裏取りが
// 取れていないため、実チップと突き合わせる場合はここを確認してください。
// 差し替えが必要になるのは「テーブルの値」だけで、処理の骨格は変わりません。
// =====================================================================

// --- Z280 ---
// ステップ幅をテーブル索引ではなく「倍率」で更新するのが特徴。
namespace Z280Adpcm {
    // ステップ倍率 (1/256 単位)。nibble の下位 3bit で選ぶ。
    inline const int StepScale[8] = { 0x0e6, 0x0e6, 0x0e6, 0x0e6, 0x133, 0x199, 0x200, 0x266 };

    inline constexpr int stepMin = 0x007f;
    inline constexpr int stepMax = 0x6000;
}

// --- Konami K53 ---
// ステップ適応を持たない固定 DPCM。指数的に並んだ差分を足していく。
namespace K53Adpcm {
    inline const int Delta[16] = {
        0, 1, 2, 4, 8, 16, 32, 64,
        -128, -64, -32, -16, -8, -4, -2, -1
    };

    // 差分をそのまま足すと 16bit に対して細かすぎるため、実機の出力段に合わせて拡大する
    inline constexpr int scale = 64;
}

// --- Konami K54 ---
// K53 と同じく固定 DPCM だが、差分が二乗的に並ぶ。
namespace K54Adpcm {
    inline const int Delta[16] = {
        0, 1, 4, 9, 16, 25, 36, 49,
        -64, -49, -36, -25, -16, -9, -4, -1
    };

    inline constexpr int scale = 256;
}

class Z280AdpcmCodec {
public:
    static void process(std::vector<int16_t>& samples);
};

class KonamiAdpcmCodec {
public:
    // isK54 = false で K53、true で K54
    static void process(std::vector<int16_t>& samples, bool isK54);
};
