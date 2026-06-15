#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opl/ProcessorOplValues.h"

class OplProcessor : public PrBase
{
    std::atomic<float>* pLevel = nullptr;
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, OplPrValue::ops> pOpMutiple = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrAr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrDr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrSl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrRr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrTl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrKsr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrKsl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrSus = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrXof = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrKor = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpAdsrEgType = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoEg = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoPmEnable = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoPms = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoPmd = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoAmEnable = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoAms = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpLfoAmd = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvAr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvDr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvRr = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvStl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvAtl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvSsl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpPitchEnvRll = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpSsgSwEnvL6 = { nullptr };
    std::array<std::atomic<float>*, OplPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
