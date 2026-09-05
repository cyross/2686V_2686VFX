#pragma once
#include <JuceHeader.h>

static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    OPM = 0, // YM2151 (X68000/Arcade)
    Count = 1, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPM: return "OPM";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPM") return OscMode::OPM;

    return OscMode::OPM; // Default
}
