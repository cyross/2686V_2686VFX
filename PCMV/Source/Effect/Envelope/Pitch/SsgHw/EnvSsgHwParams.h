#pragma once

#include "../../Amp/SsgHw/EnvSsgHwParams.h"

// SSG HW PITCH ENV のパラメータ。
//
// 波形スロット (SsgHwShape) は SSG HW AMP ENV と同じものを使う。実機の
// ハードウェアエンベロープは音量へ掛かるものだが、同じ形をピッチへ当てると
// アルペジオやトリルのような動きが作れるので、こちらは独自の追加になる。
//
// 違うのは MIN / MAX の意味だけで、音量の倍率ではなくセント値として扱う。
struct SsgHwPEnvParams {
    // Hardware Pitch Envelope Enable Switch
    bool enable = false;

    // Hardware Pitch Envelope Shape Index (SsgHwShape)
    int shape = 0;

    // Hardware Pitch Envelope Period Freq
    float period = 1.0f;

    // 振れ幅の下端 / 上端。単位はセントで、1200 セントが 1 オクターブ。
    int min = 0;

    int max = 1200;

    // 出力にスムース処理を掛けるかどうか。
    // 音量版と違い、こちらは段差がそのままピッチの飛びとして出る。
    bool smooth = false;
};
