#pragma once
#include <JuceHeader.h>

static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    OPN = 0,    // YM2203
    SSG = 1,
    Count = 2, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPN:  return "OPN";
    case OscMode::SSG:  return "SSG";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPN")  return OscMode::OPN;
    if (name == "SSG")  return OscMode::SSG;
    return OscMode::OPN; // Default
}
