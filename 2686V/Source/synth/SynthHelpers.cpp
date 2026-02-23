#include "SynthHelpers.h"

double getTargetRate(int index) {
    switch (index) {
    case 0: return 96000.0;
    case 1: return 55500.0;
    case 2: return 48000.0;
    case 3: return 44100.0;
    case 4: return 22050.0;
    case 5: return 16000.0;
    case 6: return 8000.0;
    default: return 55500.0;
    }
}
