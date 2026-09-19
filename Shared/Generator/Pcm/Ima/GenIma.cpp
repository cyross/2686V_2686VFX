#include <algorithm>

#include "./GenIma.h"

void ImaAdpcmCodec::reset()
{
    predictedValue = 0;
    stepIndex = 0;
}

uint8_t ImaAdpcmCodec::encode(int16_t pcmSample)
{
    int step = ImaAdpcm::StepSizeTable[stepIndex];
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

    // 状態の更新は decode() 側で 1 回だけ行う
    return nibble;
}

int16_t ImaAdpcmCodec::decode(uint8_t nibble)
{
    return decodeAndUpdateState(nibble);
}

int16_t ImaAdpcmCodec::decodeAndUpdateState(uint8_t nibble)
{
    int step = ImaAdpcm::StepSizeTable[stepIndex];
    int diff = step >> 3;

    if (nibble & 4) diff += step;
    if (nibble & 2) diff += (step >> 1);
    if (nibble & 1) diff += (step >> 2);

    if (nibble & 8) predictedValue -= diff;
    else            predictedValue += diff;

    predictedValue = std::clamp(predictedValue, -32768, 32767);

    stepIndex += ImaAdpcm::StepAdjustTable[nibble & 7];
    stepIndex = std::clamp(stepIndex, 0, 88);

    return (int16_t)predictedValue;
}

void ImaAdpcmCodec::process(std::vector<int16_t>& samples)
{
    ImaAdpcmCodec codec;
    codec.reset();

    for (auto& s : samples) {
        s = codec.decode(codec.encode(s));
    }
}
