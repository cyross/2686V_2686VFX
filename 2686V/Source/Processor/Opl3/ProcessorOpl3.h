#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"

class Opl3Processor : public PrBase
{
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, 4> pOpMutiple = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrBypass = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrAr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrDr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrSl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrTl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKsr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKsl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrXof = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKor = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrEgType = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoEg = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPmEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPms = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPmd = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAms = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmd = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvAr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvDr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvRr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvStl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvAtl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvSsl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPitchEnvRll = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvSteps = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvLoop = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvLoopTo = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvLoopCount = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvStl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvR1 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvL1 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvR2 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvL2 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvR3 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvL3 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvR4 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvL4 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvR5 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvL5 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvR6 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgSwEnvL6 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpMask = { nullptr, nullptr, nullptr, nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
