#pragma once
#include <JuceHeader.h>

static constexpr int MaxFmOperators = 4;

enum class OscMode
{
    WAVETABLE = 0, // 波形メモリ
    WT2 = 1, // 波形メモリ (2 系統)
    WTPLUS = 2, // 波形メモリを 32 スロット持つ拡張ウェーブテーブル
    Count = 3, // カウント用
};

static juce::String getModeName(OscMode mode) {
    switch (mode) {
    case OscMode::WAVETABLE: return "WAVETABLE";
    case OscMode::WT2: return "WT2";
    case OscMode::WTPLUS: return "WTPLUS";
    default: return "Unknown";
    }
}

static OscMode getModeFromString(const juce::String& name) {
    if (name == "WAVETABLE") return OscMode::WAVETABLE;
    if (name == "WT2") return OscMode::WT2;
    if (name == "WTPLUS") return OscMode::WTPLUS;

    return OscMode::WAVETABLE; // Default
}
