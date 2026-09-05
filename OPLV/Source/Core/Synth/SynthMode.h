#pragma once
#include <JuceHeader.h>

static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    OPL = 0, // YM3526 (2op + WaveSelect)
    OPL3 = 1, // YMF262
    Count = 2, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPL: return "OPL";
    case OscMode::OPL3: return "OPL3";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPL") return OscMode::OPL;
    if (name == "OPL3") return OscMode::OPL3;

    return OscMode::OPL; // Default
}
