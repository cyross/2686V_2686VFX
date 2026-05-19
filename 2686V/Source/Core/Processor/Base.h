#pragma once

#include <JuceHeader.h>

class PrBase
{
protected:
    void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void addPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void addOpPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix);
    void addSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
};
