#include <algorithm>
#include <cmath>

#include "./KSOpl.h"

namespace
{
    // 表の中身は音符から決まるだけで、どの実体でも同じになる。
    // 1 実体あたり 2.5KB あり、オペレータの数だけ抱えると数 MB になるので、
    // 1 組だけ作って皆で見る。
    struct KSOplTables
    {
        std::array<int, 128> keyRates{};
        std::array<std::array<float, 4>, 128> kslAttenuations{};

        KSOplTables()
        {
            // 実機 YM3812 / YMF262 の KSL レジスタは 01 と 10 が直感と逆に並んでいる。
            //   00 = 0dB/oct / 01 = 3.0dB/oct / 10 = 1.5dB/oct / 11 = 6.0dB/oct
            const std::array<float, 4> dbPerOcts = { 0.0f, 3.0f, 1.5f, 6.0f };

            for (int i = 0; i < 128; i++) {
                int octave = (i / 12) - 1;

                if (octave < 0) octave = 0;
                if (octave > 7) octave = 7;

                int noteOffset = i % 12;

                keyRates[i] = (octave * 2) + ((noteOffset > 7) ? 1 : 0);

                float octaveDiff = (float)(i - 48) / 12.0f;

                if (octaveDiff < 0.0f) octaveDiff = 0.0f;

                for (int l = 0; l < 4; l++) {
                    float totalDb = dbPerOcts[l] * octaveDiff;

                    kslAttenuations[i][l] = std::pow(10.0f, -totalDb / 20.0f);
                }
            }
        }
    };

    const KSOplTables tables;
}

void KSOpl::setParameters(const KSOplParams& params) {
	m_ksr = params.ksr;
	m_ksl = params.ksl;
}

int KSOpl::calcKeyScaleRate(const int noteNumber) const {
    int keyRate = tables.keyRates[noteNumber];

    return  m_ksr ? keyRate : (keyRate >> 2);
}

float KSOpl::calcLevelScalingGain(const int noteNumber) const {
    return m_ksl > 0 ? tables.kslAttenuations[noteNumber][std::clamp(m_ksl, 0, 3)] : 1.0f;
}
