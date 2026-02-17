#pragma once

#include <JuceHeader.h>

struct SelectItem
{
    const juce::String name;
    const int value;
};

struct Size
{
    int width;
    int height;
};

struct Flags
{
    bool isReset;
    bool isResize;
};
