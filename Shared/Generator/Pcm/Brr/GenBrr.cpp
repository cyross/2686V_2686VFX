#include <algorithm>
#include <cmath>

#include "./GenBrr.h"

int BrrCodec::predict(int filter, int prev1, int prev2)
{
    // 実機は整数シフト演算で近似している。係数はおおよそ次のとおり。
    //   filter 1 : prev1 * 15/16
    //   filter 2 : prev1 * 61/32 - prev2 * 15/16
    //   filter 3 : prev1 * 115/64 - prev2 * 13/16
    switch (filter) {
    case 0:
        return 0;
    case 1:
        return prev1 + ((-prev1) >> 4);
    case 2:
        return (prev1 * 2) + ((-prev1 * 3) >> 5) - prev2 + (prev2 >> 4);
    case 3:
        return (prev1 * 2) + ((-prev1 * 13) >> 6) - prev2 + ((prev2 * 3) >> 4);
    }

    return 0;
}

double BrrCodec::encodeBlock(
    const int16_t* src,
    int count,
    int shift,
    int filter,
    int prev1,
    int prev2,
    int16_t* dest
)
{
    double error = 0.0;

    for (int i = 0; i < count; ++i) {
        int predicted = predict(filter, prev1, prev2);

        // 残差を shift で割って 4bit に丸める
        int residual = src[i] - predicted;
        int nibble = (int)std::lround((double)residual / (double)(1 << shift));
        nibble = std::clamp(nibble, -8, 7);

        // 復号側と同じ計算で戻す
        int decoded = (nibble << shift) + predicted;

        // 実機の BRR 出力は 15bit だが、そのまま再現すると他のモードより
        // 6dB 小さくなってしまうため、ここでは 16bit でクランプするに留める。
        // (15bit の折り返しは意図的なオーバーフローを使う場面でしか効かない)
        decoded = std::clamp(decoded, -32768, 32767);

        double diff = (double)src[i] - (double)decoded;
        error += diff * diff;

        dest[i] = (int16_t)decoded;

        prev2 = prev1;
        prev1 = decoded;
    }

    return error;
}

void BrrCodec::process(std::vector<int16_t>& samples)
{
    if (samples.empty()) return;

    std::vector<int16_t> block(SnesBrr::samplesPerBlock);
    std::vector<int16_t> best(SnesBrr::samplesPerBlock);

    int prev1 = 0;
    int prev2 = 0;

    for (size_t pos = 0; pos < samples.size(); pos += SnesBrr::samplesPerBlock) {
        int count = (int)std::min((size_t)SnesBrr::samplesPerBlock, samples.size() - pos);

        double bestError = -1.0;
        int bestPrev1 = prev1;
        int bestPrev2 = prev2;

        // shift と filter の全組み合わせを試し、誤差が最小のものを採用する。
        // 実機のエンコーダも同じ考え方で総当りする。
        for (int filter = 0; filter < SnesBrr::filterCount; ++filter) {
            // 先頭ブロックは直前サンプルが無いので、予測なし(filter 0)しか使えない
            if (pos == 0 && filter != 0) continue;

            for (int shift = 0; shift <= SnesBrr::maxShift; ++shift) {
                double error = encodeBlock(&samples[pos], count, shift, filter, prev1, prev2, block.data());

                if (bestError < 0.0 || error < bestError) {
                    bestError = error;
                    std::copy(block.begin(), block.begin() + count, best.begin());
                    bestPrev1 = (count >= 1) ? best[count - 1] : prev1;
                    bestPrev2 = (count >= 2) ? best[count - 2] : prev1;
                }
            }
        }

        std::copy(best.begin(), best.begin() + count, samples.begin() + pos);

        prev1 = bestPrev1;
        prev2 = bestPrev2;
    }
}
