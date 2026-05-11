#include <cmath>
#include <vector>

#include <JuceHeader.h>

#include "SynthHelpers.h"

double getTargetRate(int index, double defaultValue) {
    switch (index) {
        case 1:
            return 96000.0;
        case 2:
            return 55500.0;
        case 3:
            return 48000.0;
        case 4:
            return 44100.0;
        case 5:
            return 22050.0;
        case 6:
            return 16000.0;
        case 7:
            return 8000.0;
    }

    return defaultValue;
}

float getTargetBitDepth(int index)
{
    // ±の解像度を設定。これにより 0.0 が必ず中央ステップに配置されます。
    switch (index) {
    case 1: return 7.0f;    // 4-bit (-7 ~ 0 ~ +7 の15段階)
    case 2: return 15.0f;   // 5-bit (-15 ~ 0 ~ +15 の31段階)
    case 3: return 31.0f;   // 6-bit (-31 ~ 0 ~ +31 の63段階)
    case 4: return 127.0f;  // 8-bit (-127 ~ 0 ~ +127 の255段階)
    case 5: return 0.0f;    // Raw (32-bit float / 量子化なし)
    case 6: return 63.0f;   // ★追加した 7-bit (OPLL)
    }

    return 127.0f;
}

float getTargetMaxVal(int index)
{
    // 4bit ADPCM は、別処理になるため対応不要
    switch (index) {
    // 32bit Float
    case 1:
        return 0.0f;      
    // 24bit Float
    case 2:
        return 8388607.0f;
    // 16bit Float
    case 3:
        return 32767.0f;   
    // 8bit Float
    case 4:
        return 127.0f;
    // 5bit Float
    case 5:
        return 15.0f;
    // 4bit Linear
    case 6:
        return 7.0f;
    }

    return 0.0f;
}
