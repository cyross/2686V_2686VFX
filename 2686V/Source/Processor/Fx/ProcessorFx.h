#pragma once

#include <JuceHeader.h>

#include "../../Effect/Fx/Fx.h"

#include "../../Core/Processor/ProcessorBase.h"

class FxProcessor : public PrBase
{
    std::atomic<float>* pBypass = nullptr;
    std::atomic<float>* pFlBypass = nullptr;
    std::atomic<float>* pFlType = nullptr;
    std::atomic<float>* pFlFreq = nullptr;
    std::atomic<float>* pFlQ = nullptr;
    std::atomic<float>* pFlMix = nullptr;
    std::atomic<float>* pEq3bBypass = nullptr;
    std::atomic<float>* pEq3bLowGainDb = nullptr;
    std::atomic<float>* pEq3bMidFreq = nullptr;
    std::atomic<float>* pEq3bMidGainDb = nullptr;
    std::atomic<float>* pEq3bHighGainDb = nullptr;
    std::atomic<float>* pEq3bMix = nullptr;
    std::atomic<float>* pVBypass = nullptr;
    std::atomic<float>* pVRate = nullptr;
    std::atomic<float>* pVDepth = nullptr;
    std::atomic<float>* pVMix = nullptr;
    std::atomic<float>* pTBypass = nullptr;
    std::atomic<float>* pTRate = nullptr;
    std::atomic<float>* pTDepth = nullptr;
    std::atomic<float>* pTMix = nullptr;
    std::atomic<float>* pMbcBypass = nullptr;
    std::atomic<float>* pMbcRate = nullptr;
    std::atomic<float>* pMbcBits = nullptr;
    std::atomic<float>* pMbcMix = nullptr;
    std::atomic<float>* pDBypass = nullptr;
    std::atomic<float>* pDTime = nullptr;
    std::atomic<float>* pDFb = nullptr;
    std::atomic<float>* pDMix = nullptr;
    std::atomic<float>* pRBypass = nullptr;
    std::atomic<float>* pRSize = nullptr;
    std::atomic<float>* pRDamp = nullptr;
    std::atomic<float>* pRMix = nullptr;

    EffectChain effects;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(juce::AudioBuffer<float>& buffer, SynthParams& params, juce::AudioProcessorValueTreeState& apvts);
    void prepare(double sampleRate);
    void clear();
    void init(juce::AudioProcessorValueTreeState& apvts);
};
