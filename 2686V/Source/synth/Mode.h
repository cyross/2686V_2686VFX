#pragma once
#include <JuceHeader.h>

static constexpr int MaxRhythmPads = 8;
static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    OPNA = 0,   // YM2608
    OPN = 1,    // YM2203
    OPL = 2,    // YM3526 (2op + WaveSelect)
    OPL3 = 3, // YMF262
    OPM = 4,   // YM2151 (X68000/Arcade)
    OPZX3 = 5,
    SSG = 6,
    WAVETABLE = 7,
    RHYTHM = 8,
    ADPCM = 9,
    BEEP = 10,
    Count = 11, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::OPNA: return "OPNA";
    case OscMode::OPN:  return "OPN";
    case OscMode::OPL:  return "OPL";
    case OscMode::OPL3: return "OPL3";
    case OscMode::OPM:  return "OPM";
    case OscMode::OPZX3:  return "OPZX3";
    case OscMode::SSG:  return "SSG";
    case OscMode::WAVETABLE: return "WAVETABLE";
    case OscMode::RHYTHM: return "RHYTHM";
    case OscMode::ADPCM:  return "ADPCM";
    case OscMode::BEEP:  return "BEEP";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "OPNA") return OscMode::OPNA;
    if (name == "OPN")  return OscMode::OPN;
    if (name == "OPL")  return OscMode::OPL;
    if (name == "OPL3") return OscMode::OPL3;
    if (name == "OPM")  return OscMode::OPM;
    if (name == "OPZX3")  return OscMode::OPZX3;
    if (name == "SSG")  return OscMode::SSG;
    if (name == "WAVETABLE") return OscMode::WAVETABLE;
    if (name == "RHYTHM") return OscMode::RHYTHM;
    if (name == "ADPCM")  return OscMode::ADPCM;
    if (name == "BEEP")  return OscMode::BEEP;
    return OscMode::OPNA; // Default
}
