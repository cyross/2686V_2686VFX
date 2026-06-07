#include "./GenPcmHelper.h"

#include "../../../Core/Synth/SynthHelpers.h"

// ADPCM(DPCM)特有の無音時ノイズ（ピー音）を打ち消すローパスフィルタ
void GenPcmHelper::lowPassFilter(std::vector<int16_t>& buffer)
{
    if (buffer.size() > 1) {
        int16_t prev = buffer[0];
        for (size_t i = 1; i < buffer.size(); ++i) {
            int16_t current = buffer[i];
            buffer[i] = (int16_t)(((int32_t)current + (int32_t)prev) / 2);
            prev = current;
        }
    }
}

float GenPcmHelper::bitReduction(float input, int qIndex)
{
    float maxVal = getTargetMaxVal(qIndex);

    if (maxVal > 0.0f) {
        float dither = (pcmDis(pcmGen) - 0.5f) * (1.0f / maxVal);

        // ディザを足してから丸めることで、無音にならずにノイズとして残る
        return std::round((input + dither) * maxVal) / maxVal;
    }

    return input;
}
