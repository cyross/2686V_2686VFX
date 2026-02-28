#pragma once

#include <JuceHeader.h>

struct SelectItem
{
    const juce::String name;
    const int value;
};

// プリセット1つ分の情報を保持する構造体
struct PresetItem
{
    juce::File file;
    juce::String fileName;
    juce::String name;
    juce::String author;
    juce::String version;
    juce::String comment;
    juce::String modeName;
    juce::Time lastModificationTime;
};
