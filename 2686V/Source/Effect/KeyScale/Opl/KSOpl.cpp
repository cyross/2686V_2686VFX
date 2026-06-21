#include <algorithm>
#include <cmath>

#include "./KSOpl.h"

const std::array<float, 4> KSOpl::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

KSOpl::KSOpl() {
    for (int i = 0; i < 128; i++) {
        int octave = (i / 12) - 1;

        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = i % 12;

        keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);

        float octaveDiff = (float)(i - 48) / 12.0f;

        if (octaveDiff < 0.0f) octaveDiff = 0.0f;

        for (int l = 0; l < 4; l++) {
            float totalDb = dbPerOcts[std::clamp(l, 0, 3)] * octaveDiff;

            kslAttenuations[i][l] = std::pow(10.0f, -totalDb / 20.0f);
        }
    }
}

void KSOpl::setParameters(const KSOplParams& params) {
	m_ksr = params.ksr;
	m_ksl = params.ksl;
}

int KSOpl::calcKeyScaleRate(const int noteNumber) const {
    int keyRate = keyRates[noteNumber];

    return  m_ksr ? keyRate : (keyRate >> 2);
}

float KSOpl::calcLevelScalingDb(const int noteNumber) const {
    return m_ksl > 0 ? kslAttenuations[noteNumber][m_ksl] : 1.0f;
}
