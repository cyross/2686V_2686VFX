#pragma once

#include <JuceHeader.h>
#include <array>
#include <algorithm>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/Base.h"
#include "../../Advanced/Curve/Params.h"

class CurveProcessor : public PrBase {
    std::atomic<float>* pEnable = nullptr;
    std::array<std::array<std::array<std::atomic<float>*, 8>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> pLogics;
    std::array<std::array<std::array<std::atomic<float>*, 8>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> pKs;
    std::array<std::array<std::array<std::array<std::atomic<float>*, 8>, 8>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> pValues;
    std::array<std::function<void(SynthParams& params, int p, int t, int vp)>, (int)CurveParams::Logic::Size> lLogics;
public:
    void init(juce::AudioProcessorValueTreeState& apvts);
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
};