#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/Base.h"

class OplProcessor : public PrBase
{
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, 2> pOpMutiple = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrBypass = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrAr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrDr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrSl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrRr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrTl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrKsr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrKsl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrSus = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrXof = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpAdsrEgType = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoEg = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoPmEnable = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoPms = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoPmd = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoAmEnable = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoAms = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpLfoAmd = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvEnable = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvAr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvDr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvRr = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvStl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvAtl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvSsl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpPitchEnvRll = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvEnable = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvSteps = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvLoop = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvLoopTo = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvLoopCount = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvStl = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvR1 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvL1 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvR2 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvL2 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvR3 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvL3 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvR4 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvL4 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvR5 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvL5 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvR6 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpSsgSwEnvL6 = { nullptr, nullptr };
    std::array<std::atomic<float>*, 2> pOpMask = { nullptr, nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
