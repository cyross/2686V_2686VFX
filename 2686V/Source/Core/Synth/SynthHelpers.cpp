#include <cmath>
#include <vector>

#include <JuceHeader.h>

#include "./SynthHelpers.h"

double getTargetRate(int index, double defaultValue) {
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
    switch (index) {
        case 1:
            return 96000.0; // 96kHz
        case 2:
            return 55500.0; // 55.5kHz
        case 3:
            return 49700.0; // 49.7kHz
        case 4:
            return 48000.0; // 48kHz
        case 5:
            return 44100.0; // 44.1kHz
        case 6:
            return 33075.0; // 33.08kHz
        case 7:
            return 32000.0; // 32kHz
        case 8:
            return 22050.0; // 22.05kHz
        case 9:
            return 16000.0; // 16kHz
        case 10:
            return 12000.0; // 12kHz
        case 11:
            return 11025.0; // 11.03kHz
        case 12:
            return 8000.0;  // 8kHz
        case 13:
            return 5551.0;  // 5.55kHz
        case 14:
            return 4410.0;  // 4kHz
        case 15:
            return 2205.0;  // 2kHz
    }

    return defaultValue;
}

float getTargetBitDepth(int index)
{
    // ±の解像度を設定。これにより 0.0 が必ず中央ステップに配置されます。
    // 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
    switch (index) {
    case 1: return 7.0f;         // 4-bit (-7 ~ 0 ~ +7 の15段階)
    case 2: return 15.0f;        // 5-bit (-15 ~ 0 ~ +15 の31段階)
    case 3: return 31.0f;        // 6-bit (-31 ~ 0 ~ +31 の63段階)
    case 4: return 63.0f;        // 7-bit (OPLL)
    case 5: return 127.0f;       // 8-bit (-127 ~ 0 ~ +127 の255段階)
    case 6: return 255.0f;       // 9-bit
    case 7: return 511.0f;       // 10-bit
    case 8: return 4095.0f;      // 12-bit
    case 9: return 16383.0f;     // 16-bit
    case 10: return 524287.0f;   // 20-bit
    case 11: return 8388607.0f;  // 24-bit
    case 12: return 0.0f;        // Raw (32-bit float / 量子化なし)
    }

    return 127.0f;
}

float getTargetMaxVal(int index)
{
    // 4bit ADPCM (index=13) と 1bit DPCM (index=14)は、別処理になるため対応不要
    // 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM, 13: 4bit ADPCM, 14: 1bit DPCM
    switch (index) {
    // 32bit Float
    case 1:
        return 0.0f;      
    // 24bit Float
    case 2:
        return 8388607.0f;
    // 20bit Float
    case 3:
        return 524287.0f;
    // 16bit Float
    case 4:
        return 32767.0f;   
    // 12bit Float
    case 5:
        return 4095.0f;
    // 10bit Float
    case 6:
        return 511.0f;
    // 9bit Float
    case 7:
        return 255.0f;
    // 8bit Float
    case 8:
        return 127.0f;
    // 7bit Float
    case 9:
        return 63.0f;
    // 6bit Float
    case 10:
        return 31.0f;
    // 5bit Float
    case 11:
        return 15.0f;
    // 4bit Linear
    case 12:
        return 7.0f;
    }

    return 0.0f;
}
