#pragma once
#include <JuceHeader.h>

static constexpr int MaxRhythmPads = 6;
static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    OPNA = 0,   // YM2608
    SSG = 1,
    RHYTHM = 2,
    ADPCM = 3,
    Count = 4, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPNA: return "OPNA";
    case OscMode::SSG:  return "SSG";
    case OscMode::RHYTHM: return "RHYTHM";
    case OscMode::ADPCM:  return "ADPCM";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPNA") return OscMode::OPNA;
    if (name == "SSG")  return OscMode::SSG;
    if (name == "RHYTHM") return OscMode::RHYTHM;
    if (name == "ADPCM")  return OscMode::ADPCM;
    return OscMode::OPNA; // Default
}
