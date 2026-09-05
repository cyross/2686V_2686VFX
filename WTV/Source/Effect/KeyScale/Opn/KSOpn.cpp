#include <algorithm>

#include "./KSOpn.h"

namespace
{
    // 表の中身は音符から決まるだけで、どの実体でも同じになる。
    // オペレータの数だけ抱えると無視できない量になるので、1 組だけ作って皆で見る。
    struct KSOpnTables
    {
        std::array<int, 128> keyRates{};

        KSOpnTables()
        {
            for (int i = 0; i < 128; i++) {
                int octave = (i / 12) - 1;

                if (octave < 0) octave = 0;
                if (octave > 7) octave = 7;

                int noteOffset = i % 12;

                keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
            }
        }
    };

    const KSOpnTables tables;
}

void KSOpn::setParameters(const KSOpnParams& params) {
	m_ks = params.ks;
    m_shift = 3 - std::clamp(m_ks, 0, 3);
}

int KSOpn::calcKeyScaleRate(const int noteNumber) const {
    return tables.keyRates[noteNumber] >> m_shift;
}

float KSOpn::calcLevelScalingDb(const int noteNumber) const {
    return 0.0f;
}
