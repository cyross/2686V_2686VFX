#pragma once

#include <JuceHeader.h>

#include "../../Effect/Fx/Core.h"

#include "../../Core/Processor/Base.h"

class FxProcessor : PrBase
{
    EffectChain effects;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
    void prepare(double sampleRate);
    void clear();
};
