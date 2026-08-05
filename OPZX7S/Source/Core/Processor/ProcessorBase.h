#pragma once

#include <JuceHeader.h>

#include "../Synth/SynthParams.h"

class PrBase
{
public:
    void virtual createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) {}
    void virtual processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) {}
};
