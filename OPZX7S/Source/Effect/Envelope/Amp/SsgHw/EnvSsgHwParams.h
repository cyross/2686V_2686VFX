#pragma once

struct SsgHwEnvParams {
    // Hardware Envelope Enable Switch
    bool enable = false;

    // Hardware Envelope Shape Index
    int shape = 0;

    // Hardware Envelope Period Freq
    float period = 1.0f;

    float min = 0.0f;

    float max = 1.0f;

    // 出力にスムース処理を掛けるかどうか。
    // 波形の折り返しで生じる段差がブツブツ音の原因なので、
    // Period が大きいほど効果が分かりやすい。
    bool smooth = false;
};
