#pragma once

#include <JuceHeader.h>

#include "../Synth/SynthParams.h"

class PrBase
{
public:
    void virtual createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) {}
    void virtual processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) {}
protected:
    void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void addOpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix);
    void addPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void addOpPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix);
    void addSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void addOpSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix);
    void addOpzx7LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void addOpOpzx7LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix);
};
