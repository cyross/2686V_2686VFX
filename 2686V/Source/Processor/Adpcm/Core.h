#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/PrBase.h"

class AdpcmProcessor : PrBase
{
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
};
