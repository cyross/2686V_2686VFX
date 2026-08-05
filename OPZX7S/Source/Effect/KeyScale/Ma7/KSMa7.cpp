#include <algorithm>
#include <cmath>

#include "./KSMa7.h"

// KSLの減衰カーブ定義 (MA-7用: 4段階)
const std::array<float, 4> KSMa7::m_dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

KSMa7::KSMa7() {
    for (int i = 0; i < 128; i++) {
        int octave = (i / 12) - 1;

        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = i % 12;

        keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);

        // C3(48) を基準とし、それより高い音符で音量を減衰させる
        float octaveDiff = (float)(i - 48) / 12.0f;

        if (octaveDiff < 0.0f) octaveDiff = 0.0f;

        for (int l = 0; l < 4; l++) {
            float octaveDiff = (float)(i - 48) / 12.0f;

            if (octaveDiff < 0.0f) octaveDiff = 0.0f;

            totalDbs[i][l] = m_dbPerOcts[l] * octaveDiff;
        }
    }
}

void KSMa7::setParameters(const KSMa7Params& params) {
	m_ksr = params.ksr;
	m_ksl = params.ksl;
}

int KSMa7::calcKeyScaleRate(const int noteNumber) const {
    int keyRate = keyRates[noteNumber];

    return  m_ksr ? keyRate : (keyRate >> 2);
}

float KSMa7::calcLevelScalingDb(const int noteNumber) const {
    return totalDbs[noteNumber][std::clamp(m_ksl, 0, 3)];
}
