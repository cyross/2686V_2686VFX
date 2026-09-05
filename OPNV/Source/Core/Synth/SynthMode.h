#pragma once
#include <JuceHeader.h>

static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    OPNA = 0, // YM2608
    OPN = 1, // YM2203
    Count = 2, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPNA: return "OPNA";
    case OscMode::OPN: return "OPN";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPNA") return OscMode::OPNA;
    if (name == "OPN") return OscMode::OPN;

    return OscMode::OPNA; // Default
}
