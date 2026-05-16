#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/Base.h"

class Opzx7Processor : PrBase
{
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
};
