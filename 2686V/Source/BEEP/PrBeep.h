#pragma once

#include <JuceHeader.h>
#include "../synth/SynthParams.h"
#include "../processor/PrBase.h"

class BeepProcessor : PrBase {
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
};