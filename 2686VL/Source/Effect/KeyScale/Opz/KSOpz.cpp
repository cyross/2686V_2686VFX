#include <algorithm>

#include "./KSOpz.h"

// KSLの減衰カーブ定義 (OPZ用: 8段階) ※値は仮の近似値です
const std::array<float, 8> KSOpz::m_dbPerOcts = { 0.0f, 1.5f, 3.0f, 4.5f, 6.0f, 7.5f, 9.0f, 12.0f };

KSOpz::KSOpz() {
    for (int i = 0; i < 128; i++) {
        int octave = (i / 12) - 1;

        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = i % 12;

        keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);

        // C3(48) を基準とし、それより高い音符で音量を減衰させる
        float octaveDiff = (float)(i - 48) / 12.0f;

        if (octaveDiff < 0.0f) octaveDiff = 0.0f;

        octaveDiffs[i] = octaveDiff;
    }
}

void KSOpz::setParameters(const KSOpzParams& params) {
	m_ksr = params.ksr;
	m_ksl = params.ksl;

    m_shift = 3 - std::clamp(m_ksr, 0, 3);
    m_depth = std::clamp(m_ksl, 0, 99) / 99.0f;
}

int KSOpz::calcKeyScaleRate(const int noteNumber) const {
    return keyRates[noteNumber] >> m_shift;
}

float KSOpz::calcLevelScalingDb(const int noteNumber) const {
    if (m_ksl <= 0) return 0.0f;

    return m_maxDbPerOct * octaveDiffs[noteNumber] * m_depth;
}
