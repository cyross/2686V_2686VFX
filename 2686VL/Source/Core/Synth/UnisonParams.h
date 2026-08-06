#pragma once

struct UnisonParams {
    int voices = 1;        // 1 to 8
    int detuneCents = 0;   // cents
    float spread = 1.0f;   // 0.0 to 1.0 (Stereo width)
};
