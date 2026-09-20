#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"

// 音源ごとのパラメータを APVTS から SynthCoreParams へ写す。
// どの音源を鳴らすか (mode) はプラグインごとに番号が違うので、ここでは扱わない。
class PrBase
{
public:
    void virtual createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) {}
    void virtual processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) {}
};
