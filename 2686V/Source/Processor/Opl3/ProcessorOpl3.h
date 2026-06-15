#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"

class Opl3Processor : public PrBase
{
    std::atomic<float>* pLevel = nullptr;
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpMutiple = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrAr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrDr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrSl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrRr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrTl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrKsr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrKsl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrXof = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrKor = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpAdsrEgType = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoEg = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoPmEnable = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoPms = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoPmd = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoAmEnable = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoAms = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpLfoAmd = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvEnable = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvAr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvDr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvRr = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvStl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvAtl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvSsl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpPitchEnvRll = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvEnable = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpSsgSwEnvL6 = { nullptr };
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
