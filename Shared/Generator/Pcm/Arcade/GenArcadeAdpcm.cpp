#include <algorithm>
#include <cmath>

#include "./GenArcadeAdpcm.h"

// =====================================================================
// YMZ280B
// =====================================================================
void Ymz280bAdpcmCodec::process(std::vector<int16_t>& samples)
{
    int predicted = 0;
    int step = Ymz280bAdpcm::stepMin;

    for (auto& s : samples) {
        int diff = (int)s - predicted;
        int sign = 0;

        if (diff < 0) {
            sign = 8;
            diff = -diff;
        }

        // 復号値は step * (delta * 2 + 1) / 8 なので、そこから delta を逆算する
        int delta = ((diff * 8) / step - 1) / 2;
        delta = std::clamp(delta, 0, 7);

        int nibble = sign | delta;

        // --- 復号 (実機と同じ手順) ---
        int amount = (step * ((delta * 2) + 1)) >> 3;

        if (nibble & 8) predicted -= amount;
        else            predicted += amount;

        predicted = std::clamp(predicted, -32768, 32767);

        // ステップ幅は倍率で更新する
        step = (step * Ymz280bAdpcm::StepScale[delta]) >> 8;
        step = std::clamp(step, Ymz280bAdpcm::stepMin, Ymz280bAdpcm::stepMax);

        s = (int16_t)predicted;
    }
}

// =====================================================================
// Konami K053260 / K054539
// =====================================================================
void KonamiAdpcmCodec::process(std::vector<int16_t>& samples, bool isK054539)
{
    const int* table = isK054539 ? K054539Adpcm::Delta : K053260Adpcm::Delta;
    const int scale = isK054539 ? K054539Adpcm::scale : K053260Adpcm::scale;

    int predicted = 0;

    for (auto& s : samples) {
        int diff = (int)s - predicted;

        // 適応が無いので、16 個の固定差分から一番近いものを選ぶだけ
        int bestIndex = 0;
        int bestError = -1;

        for (int i = 0; i < 16; ++i) {
            int candidate = std::clamp(predicted + table[i] * scale, -32768, 32767);
            int error = std::abs((int)s - candidate);

            if (bestError < 0 || error < bestError) {
                bestError = error;
                bestIndex = i;
            }
        }

        predicted = std::clamp(predicted + table[bestIndex] * scale, -32768, 32767);

        s = (int16_t)predicted;
    }
}
