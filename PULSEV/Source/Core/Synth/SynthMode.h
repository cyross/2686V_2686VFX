#pragma once
#include <JuceHeader.h>

static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    SSG = 0, // AY-3-8910 系
    BEEP = 1, // ビープ
    Count = 2, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::SSG: return "SSG";
    case OscMode::BEEP: return "BEEP";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "SSG") return OscMode::SSG;
    if (name == "BEEP") return OscMode::BEEP;

    return OscMode::SSG; // Default
}
