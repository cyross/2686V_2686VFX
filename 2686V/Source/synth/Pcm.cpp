#include "Pcm.h"
#include "SynthHelpers.h"

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

// ADPCM特有の無音時ノイズ（ピー音）を打ち消すローパスフィルタ
void adpcmLowPassFilter(std::vector<int16_t>& adpcmBuffer)
{
    if (adpcmBuffer.size() > 1) {
        int16_t prev = adpcmBuffer[0];
        for (size_t i = 1; i < adpcmBuffer.size(); ++i) {
            int16_t current = adpcmBuffer[i];
            adpcmBuffer[i] = (int16_t)(((int32_t)current + (int32_t)prev) / 2);
            prev = current;
        }
    }
}

float bitReduction(float input, int qIndex)
{
    float maxVal = getTargetMaxVal(qIndex);

    if (maxVal > 0.0f) {
        float dither = ((float)std::rand() / RAND_MAX - 0.5f) * (1.0f / maxVal);

        // ディザを足してから丸めることで、無音にならずにノイズとして残る
        return std::round((input + dither) * maxVal) / maxVal;
    }

    return input;
}
