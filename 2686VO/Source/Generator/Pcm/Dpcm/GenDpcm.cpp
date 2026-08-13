#include <cstdint>
#include <cmath>

#include "./GenDpcm.h"

void DpcmCodec::reset()
{
    counter = 64; // 中心値へリセット
}

// Encode: 16bit PCM -> 1bit DPCM (ファミコン風デルタ変調)
uint8_t DpcmCodec::encode(int16_t pcmSample) {
    // 16bit PCM (-32768〜32767) を、FCの7bitカウンタ (0〜127) スケールに変換して目標値を算出
    // (pcmSample + 32768) の範囲は 0〜65535。これを512で割ると 0〜127 になる。
    int targetCounter = (pcmSample + 32768) >> 9;

    uint8_t bit = 0;
    // 現在のカウンタより目標値が大きければ 1 (加算)、小さければ 0 (減算)
    if (targetCounter > counter) {
        bit = 1;
    }
    else {
        bit = 0;
    }

    decodeAndUpdateState(bit); // 内部状態を更新

    return bit;
}

// Decode: 1bit DPCM -> 16bit PCM
int16_t DpcmCodec::decode(uint8_t bit) {
    return decodeAndUpdateState(bit);
}

int16_t DpcmCodec::decodeAndUpdateState(uint8_t bit) {
    // ファミコンのDPCM仕様: データが1なら +2、0なら -2
    // カウンタの範囲(0〜127)をはみ出しそうになる場合は増減させない（仕様上のクリップ動作）
    if (bit & 1) {
        if (counter <= 125) {
            counter += 2;
        }
    }
    else {
        if (counter >= 2) {
            counter -= 2;
        }
    }

    // 内部カウンタ(0〜127)を 16bit PCM (-32768〜32767) にスケール変換して返す
    // 中心値64を基準として、1ステップあたり512倍して波形を作る
    return (int16_t)((counter - 64) * 512);
}
