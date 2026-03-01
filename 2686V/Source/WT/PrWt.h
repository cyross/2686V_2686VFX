#pragma once

#include <JuceHeader.h>

#include "../synth/SynthParams.h"
#include "../processor/PrBase.h"

class WtProcessor : PrBase
{
public:
    void createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& code, const juce::String& name);
    template <int I>
    void processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& code);
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
};
