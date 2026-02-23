#pragma once

#include <cmath>

// --- YM2608 ADPCM Algorithm Implementation ---
namespace OpnaAdpcm {
    // ヘッダインクルード時の重複定義を防ぐため inline を付与
    inline const int StepAdjustTable[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };
    inline const int StepSizeTable[49] = {
        16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66,
        73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253,
        279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876,
        963, 1060, 1166, 1282, 1411, 1552
    };
}

class Ym2608AdpcmCodec {
private:
    int predictedValue = 0;
    int stepIndex = 0;

public:
    void reset();
    uint8_t encode(int16_t pcmSample);
    int16_t decode(uint8_t nibble);
private:
    int16_t decodeAndUpdateState(uint8_t nibble);
};
