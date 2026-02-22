#pragma once

#include <JuceHeader.h>

#include "../synth/SynthParams.h"
#include "../core/PrBase.h"

class RhythmProcessor : PrBase
{
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
};
