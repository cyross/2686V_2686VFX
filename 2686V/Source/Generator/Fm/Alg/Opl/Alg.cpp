#include "./Alg.h"
#include <algorithm>

float OplAlg::process(std::array<OpGetSampleFunc, 2>& gsFuncs) const {
    float out1, out2;
    float finalOut = 0.0f;

    gsFuncs[0](out1, 0.0f);

    if (idx == 0) { // Serial (FM)
        gsFuncs[1](out2, out1);

        finalOut = out2;
    }
    else { // Parallel (AM)
        gsFuncs[1](out2, 0.0f);

        finalOut = (out1 + out2) * 0.5f;
    }

    // =======================================================
    // 複数のキャリアが加算されても1.0を超えないように音量を調整
    // =======================================================
    return finalOut * 0.25f;
}
