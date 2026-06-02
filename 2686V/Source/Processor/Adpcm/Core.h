#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/Base.h"

class AdpcmProcessor : public PrBase
{
    std::atomic<float>* pLevel = nullptr;
    std::atomic<float>* pPan = nullptr;
    std::atomic<float>* pLoop = nullptr;
    std::atomic<float>* pQualityMode = nullptr;
    std::atomic<float>* pRateIndex = nullptr;
    std::atomic<float>* pPcmOffset = nullptr;
    std::atomic<float>* pPcmRatio = nullptr;

    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;

    std::atomic<float>* pAdsrBypass = nullptr;
    std::atomic<float>* pAdsrStl = nullptr;
    std::atomic<float>* pAdsrAr = nullptr;
    std::atomic<float>* pAdsrDr = nullptr;
    std::atomic<float>* pAdsrSl = nullptr;
    std::atomic<float>* pAdsrRr = nullptr;

    std::atomic<float>* pPitchAdsrBypass = nullptr;
    std::atomic<float>* pPitchAdsrAr = nullptr;
    std::atomic<float>* pPitchAdsrDr = nullptr;
    std::atomic<float>* pPitchAdsrRr = nullptr;
    std::atomic<float>* pPitchAdsrStl = nullptr;
    std::atomic<float>* pPitchAdsrAtl = nullptr;
    std::atomic<float>* pPitchAdsrSsl = nullptr;
    std::atomic<float>* pPitchAdsrRll = nullptr;

    std::atomic<float>* pSsgSwEnvBypass = nullptr;
    std::atomic<float>* pSsgSwEnvSteps = nullptr;
    std::atomic<float>* pSsgSwEnvLoop = nullptr;
    std::atomic<float>* pSsgSwEnvLoopTo = nullptr;
    std::atomic<float>* pSsgSwEnvLoopCount = nullptr;
    std::atomic<float>* pSsgSwEnvDr = nullptr;
    std::atomic<float>* pSsgSwEnvRr = nullptr;
    std::atomic<float>* pSsgSwEnvR1 = nullptr;
    std::atomic<float>* pSsgSwEnvR2 = nullptr;
    std::atomic<float>* pSsgSwEnvR3 = nullptr;
    std::atomic<float>* pSsgSwEnvR4 = nullptr;
    std::atomic<float>* pSsgSwEnvR5 = nullptr;
    std::atomic<float>* pSsgSwEnvR6 = nullptr;
    std::atomic<float>* pSsgSwEnvStl = nullptr;
    std::atomic<float>* pSsgSwEnvL1 = nullptr;
    std::atomic<float>* pSsgSwEnvL2 = nullptr;
    std::atomic<float>* pSsgSwEnvL3 = nullptr;
    std::atomic<float>* pSsgSwEnvL4 = nullptr;
    std::atomic<float>* pSsgSwEnvL5 = nullptr;
    std::atomic<float>* pSsgSwEnvL6 = nullptr;

    std::atomic<float>* pMultiple = nullptr;
    std::atomic<float>* pMultipleRatio = nullptr;
    std::atomic<float>* pDetune = nullptr;
    std::atomic<float>* pDetune2 = nullptr;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
