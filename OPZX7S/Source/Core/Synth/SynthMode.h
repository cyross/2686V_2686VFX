#pragma once
#include <JuceHeader.h>

enum class OscMode
{
    OPZX7 = 0,
    Count = 1, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPZX7:  return "OPZX7";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPZX7")  return OscMode::OPZX7;
    return OscMode::OPZX7; // Default
}
