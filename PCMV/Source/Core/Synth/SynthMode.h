#pragma once
#include <JuceHeader.h>

static constexpr int MaxRhythmPads = 8;
static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    RHYTHM = 0, // リズム音源
    ADPCM = 1, // ADPCM
    ADPCMPLUS = 2, // PCM を 32 スロット持つ拡張 ADPCM。WT+ の ADPCM 版。
    Count = 3, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::RHYTHM: return "RHYTHM";
    case OscMode::ADPCM: return "ADPCM";
    case OscMode::ADPCMPLUS: return "ADPCM+";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "RHYTHM") return OscMode::RHYTHM;
    if (name == "ADPCM") return OscMode::ADPCM;
    if (name == "ADPCM+") return OscMode::ADPCMPLUS;

    return OscMode::RHYTHM; // Default
}
