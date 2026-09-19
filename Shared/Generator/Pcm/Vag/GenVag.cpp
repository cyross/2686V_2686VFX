#include <algorithm>
#include <cmath>

#include "./GenVag.h"

double PsxAdpcmCodec::encodeBlock(
    const int16_t* src,
    int count,
    int shift,
    int filter,
    int prev1,
    int prev2,
    int16_t* dest
)
{
    const PsxAdpcm::Filter& f = PsxAdpcm::filters[filter];

    double error = 0.0;

    for (int i = 0; i < count; ++i) {
        // 予測値 (係数は 1/64 単位)
        int predicted = (prev1 * f.f0 + prev2 * f.f1) >> 6;

        // 復号は nibble を上位に寄せてから shift で戻すため、
        // 符号化側も同じ重み (1 << (12 - shift)) で丸める
        int weight = 1 << (12 - shift);
        int residual = src[i] - predicted;
        int nibble = (int)std::lround((double)residual / (double)weight);
        nibble = std::clamp(nibble, -8, 7);

        int decoded = (nibble * weight) + predicted;
        decoded = std::clamp(decoded, -32768, 32767);

        double diff = (double)src[i] - (double)decoded;
        error += diff * diff;

        dest[i] = (int16_t)decoded;

        prev2 = prev1;
        prev1 = decoded;
    }

    return error;
}

void PsxAdpcmCodec::process(std::vector<int16_t>& samples, int filterCount)
{
    if (samples.empty()) return;
    if (filterCount < 1) filterCount = 1;
    if (filterCount > 5) filterCount = 5;

    std::vector<int16_t> block(PsxAdpcm::samplesPerBlock);
    std::vector<int16_t> best(PsxAdpcm::samplesPerBlock);

    int prev1 = 0;
    int prev2 = 0;

    for (size_t pos = 0; pos < samples.size(); pos += PsxAdpcm::samplesPerBlock) {
        int count = (int)std::min((size_t)PsxAdpcm::samplesPerBlock, samples.size() - pos);

        double bestError = -1.0;
        int bestPrev1 = prev1;
        int bestPrev2 = prev2;

        // ブロックごとに shift と filter を総当りして誤差最小を選ぶ
        for (int filter = 0; filter < filterCount; ++filter) {
            // 先頭ブロックは直前サンプルが無いので予測なしのみ
            if (pos == 0 && filter != 0) continue;

            for (int shift = 0; shift <= PsxAdpcm::maxShift; ++shift) {
                double error = encodeBlock(&samples[pos], count, shift, filter, prev1, prev2, block.data());

                if (bestError < 0.0 || error < bestError) {
                    bestError = error;
                    std::copy(block.begin(), block.begin() + count, best.begin());
                    bestPrev1 = best[count - 1];
                    bestPrev2 = (count >= 2) ? best[count - 2] : prev1;
                }
            }
        }

        std::copy(best.begin(), best.begin() + count, samples.begin() + pos);

        prev1 = bestPrev1;
        prev2 = bestPrev2;
    }
}
