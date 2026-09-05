#pragma once
#include <JuceHeader.h>

static constexpr int MaxRhythmPads = 8;
static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    RHYTHM = 0, // リズム音源
    ADPCM = 1, // ADPCM
    Count = 2, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::RHYTHM: return "RHYTHM";
    case OscMode::ADPCM: return "ADPCM";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "RHYTHM") return OscMode::RHYTHM;
    if (name == "ADPCM") return OscMode::ADPCM;

    return OscMode::RHYTHM; // Default
}
