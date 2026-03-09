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
    switch (index) {
        case 1:
            return 15.0f;
        case 2:
            return 31.0f;
        case 3:
            return 63.0f;
        case 4:
            return 255.0f;
        case 5:
            return 0.0f;
        case 6:
            return 127.0f;  // 7-bit (OPLL / 128 steps)
    }

    return 255.0f;
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
