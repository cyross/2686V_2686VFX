#include <algorithm>
#include <cmath>

#include "./GenArcadeAdpcm.h"

// =====================================================================
// Z280
// =====================================================================
void Z280AdpcmCodec::process(std::vector<int16_t>& samples)
{
    int predicted = 0;
    int step = Z280Adpcm::stepMin;

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
        step = (step * Z280Adpcm::StepScale[delta]) >> 8;
        step = std::clamp(step, Z280Adpcm::stepMin, Z280Adpcm::stepMax);

        s = (int16_t)predicted;
    }
}

// =====================================================================
// Konami K53 / K54
// =====================================================================
void KonamiAdpcmCodec::process(std::vector<int16_t>& samples, bool isK54)
{
    const int* table = isK54 ? K54Adpcm::Delta : K53Adpcm::Delta;
    const int scale = isK54 ? K54Adpcm::scale : K53Adpcm::scale;

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
