#pragma once

#include <JuceHeader.h>

class CustomTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Path getIconPath(const juce::String& name, juce::Rectangle<float> area);
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override;
};
