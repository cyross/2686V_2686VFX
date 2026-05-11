#pragma once

#include <JuceHeader.h>

class CustomTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override;
    juce::Colour getTabHeaderColor(int tabIndex);
};
