#include "Pcm.h"

void Ym2608AdpcmCodec::reset()
{
    predictedValue = 0;
    stepIndex = 0;
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

    decodeAndUpdateState(nibble); // Update internal state
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
