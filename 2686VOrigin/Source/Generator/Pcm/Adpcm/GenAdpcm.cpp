#include <cstdint>
#include <cmath>

#include "./GenAdpcm.h"

void Ym2608AdpcmCodec::reset()
{
    predictedValue = 0;
    stepIndex = 0;
}

void Ym2608AdpcmCodec::process(std::vector<int16_t>& samples)
{
    Ym2608AdpcmCodec codec;
    codec.reset();

    for (auto& s : samples) {
        s = codec.decode(codec.encode(s));
    }
}

// Encode: 16bit PCM -> 4bit ADPCM
uint8_t Ym2608AdpcmCodec::encode(int16_t pcmSample) {
    int step = OpnaAdpcm::StepSizeTable[stepIndex];
    int diff = pcmSample - predictedValue;
    uint8_t nibble = 0;

    if (diff < 0) {
        nibble = 8;
        diff = -diff;
    }

    // step / step2 / step4 との貪欲比較は、デコーダの再構成値
    // (2c+1) * step/8 に対する最適な量子化になっている。
    // デコーダが足す step>>3 を先に引くと 1 段ずつ下振れするので触らないこと。
    int tempStep = step;
    if (diff >= tempStep) {
        nibble |= 4;
        diff -= tempStep;
    }
    tempStep >>= 1;
    if (diff >= tempStep) {
        nibble |= 2;
        diff -= tempStep;
    }
    tempStep >>= 1;
    if (diff >= tempStep) {
        nibble |= 1;
    }

    // 状態(予測値とステップ)の更新は decode() 側で 1 回だけ行う。
    // ここで更新すると decode(encode(x)) で二重に適用されてしまう。
    return nibble;
}

// Decode: 4bit ADPCM -> 16bit PCM
int16_t Ym2608AdpcmCodec::decode(uint8_t nibble) {
    return decodeAndUpdateState(nibble);
}

int16_t Ym2608AdpcmCodec::decodeAndUpdateState(uint8_t nibble) {
    int step = OpnaAdpcm::StepSizeTable[stepIndex];
    int diff = step >> 3;

    if (nibble & 4) diff += step;
    if (nibble & 2) diff += (step >> 1);
    if (nibble & 1) diff += (step >> 2);

    if (nibble & 8) predictedValue -= diff;
    else            predictedValue += diff;

    if (predictedValue > 32767) predictedValue = 32767;
    else if (predictedValue < -32768) predictedValue = -32768;

    stepIndex += OpnaAdpcm::StepAdjustTable[nibble & 7];
    if (stepIndex < 0) stepIndex = 0;
    else if (stepIndex > 48) stepIndex = 48;

    return (int16_t)predictedValue;
}
