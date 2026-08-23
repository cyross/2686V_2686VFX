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
};
