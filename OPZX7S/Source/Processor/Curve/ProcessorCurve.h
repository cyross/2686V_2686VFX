#pragma once

#include <JuceHeader.h>
#include <array>
#include <algorithm>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Advanced/Curve/AdvancedCurveParams.h"
#include "./ProcessorCurveValues.h"

class CurveProcessor : public PrBase {
    bool forceAll = true;
    int currentPosition = 0;
	int currentTarget = 0;
    std::atomic<float>* pEnable = nullptr;
    std::array<std::array<std::array<std::atomic<float>*, CurvePrValue::params>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> pLogics;
    std::array<std::array<std::array<std::atomic<float>*, CurvePrValue::params>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> pKs;
    std::array<std::array<std::array<std::array<std::atomic<float>*, CurvePrValue::values>, CurvePrValue::params>, (int)CurveParams::Target::Size>, (int)CurveParams::Position::Size> pValues;
public:
    void init(juce::AudioProcessorValueTreeState& apvts);
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void processBlockAllParams(SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
    void updateCurrent(int position, int target);
	void resetProcessBlock();
};