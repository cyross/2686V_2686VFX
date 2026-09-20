#include <algorithm>
#include <cmath>

#include "./KSMa7.h"

namespace
{
    // 表の中身は音符から決まるだけで、どの実体でも同じになる。
    // 1 実体あたり 2.5KB あり、オペレータの数だけ抱えると数 MB になるので、
    // 1 組だけ作って皆で見る。
    struct KSMa7Tables
    {
        std::array<int, 128> keyRates{};
        std::array<std::array<float, 4>, 128> totalDbs{};

        KSMa7Tables()
        {
            // KSLの減衰カーブ定義 (MA-7用: 4段階)
            // MA-3/MA-5/MA-7 は OPL 系の派生なので、KSL のレジスタ順も OPL と同じく
            //   00 = 0dB/oct / 01 = 3.0dB/oct / 10 = 1.5dB/oct / 11 = 6.0dB/oct
            const std::array<float, 4> dbPerOcts = { 0.0f, 3.0f, 1.5f, 6.0f };

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
                    totalDbs[i][l] = dbPerOcts[l] * octaveDiff;
                }
            }
        }
    };

    const KSMa7Tables tables;
}

void KSMa7::setParameters(const KSMa7Params& params) {
	m_ksr = params.ksr;
	m_ksl = params.ksl;
}

int KSMa7::calcKeyScaleRate(const int noteNumber) const {
    int keyRate = tables.keyRates[noteNumber];

    return  m_ksr ? keyRate : (keyRate >> 2);
}

float KSMa7::calcLevelScalingDb(const int noteNumber) const {
    return tables.totalDbs[noteNumber][std::clamp(m_ksl, 0, 3)];
}
