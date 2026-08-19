#include <algorithm>

#include "./KSOpn.h"

KSOpn::KSOpn() {
    for (int i = 0; i < 128; i++) {
        int octave = (i / 12) - 1;

        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = i % 12;

        keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
    }
}

void KSOpn::setParameters(const KSOpnParams& params) {
	m_ks = params.ks;
    m_shift = 3 - std::clamp(m_ks, 0, 3);
}

int KSOpn::calcKeyScaleRate(const int noteNumber) const {
    return keyRates[noteNumber] >> m_shift;
}

float KSOpn::calcLevelScalingDb(const int noteNumber) const {
    return 0.0f;
}
