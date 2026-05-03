#pragma once

#include <JuceHeader.h>

class PrBase
{
protected:
    void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
};
