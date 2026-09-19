#pragma once

#include <cstdint>
#include <vector>
#include <cmath>

class DpcmCodec {
private:
    // ファミコンのDPCMは 7bit(0〜127) のデルタカウンタを使用する
    // 初期値は中心の64とする
    int counter = 64;

public:
    void reset();

    // int16 列をまとめてエンコード → デコードして上書きする
    static void process(std::vector<int16_t>& samples);
    // 16bit PCMを入力し、1bit(0 または 1)のDPCMデータを出力する
    uint8_t encode(int16_t pcmSample);
    // 1bitのDPCMデータを入力し、16bit PCMとしてデコードする
    int16_t decode(uint8_t bit);

private:
    int16_t decodeAndUpdateState(uint8_t bit);
};

// --- DPCMになるモード番号 ---
static const int dpcmMode = 14;
