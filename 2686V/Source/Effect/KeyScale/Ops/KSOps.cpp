#include <algorithm>
#include <cmath>

#include "./KSOps.h"

KSOps::KSOps() {
    for (int i = 0; i < 128; i++) {
        int octave = (i / 12) - 1;

        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = i % 12;

        keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
    }

    for (int xx = -127; xx < 127; xx++) { // noteNumber(0～127) - m_ksBp(0～127)
        int xd = xx + 127;
        float diff = (float)xx;
        float x = std::abs(diff) / 12.0f; // 1オクターブを1.0とする
        float maxDbPerOct = 48.0f; // 1オクターブあたりの最大変化量

        xs[xd] = maxDbPerOct * x;
        xpows[xd] = maxDbPerOct * (std::pow(2.0f, x) - 1.0f);
    }
}

void KSOps::setParameters(const KSOpsParams& params) {
	m_ksBp = params.ksBp;
	m_ksLc = params.ksLc;
	m_ksRc = params.ksRc;
	m_ksLd = params.ksLd;
	m_ksRd = params.ksRd;
	m_ksRs = params.ksRs;
}

int KSOps::calcKeyScaleRate(const int noteNumber) const {
    // Rate Scaling は 0〜7 の値を持つ。高音になるほどレートに加算される
    if (m_ksRs == 0) return 0;

    int octave = (noteNumber / 12) - 1;

    if (octave < 0) octave = 0;

    // OPSのKSRの近似計算
    return (octave * m_ksRs) >> 2;
}

float KSOps::calcLevelScalingDb(const int noteNumber) const {
    // --- OPS (DX7) モード ---
    if (noteNumber == m_ksBp) return 0.0f;

    int xd = (noteNumber - m_ksBp) + 127;
    float diff = (float)(noteNumber - m_ksBp);
    int curve = (diff < 0.0f) ? m_ksLc : m_ksRc;
    float depth = (diff < 0.0f) ? m_ksLd : m_ksRd;

    if (depth <= 0.0f) return 0.0f;

    switch (curve) {
    case 0: // -LIN (距離に応じて直線的に減衰)
        return xs[xd] * depth;
    case 1: // -EXP (距離に応じて指数的に減衰)
        return xpows[xd] * depth;
    case 2: // +EXP (距離に応じて指数的に増幅)
        return -xpows[xd] * depth;
    case 3: // +LIN (距離に応じて直線的に増幅)
        return -xs[xd] * depth;
    }

    return 0.0f;
}
