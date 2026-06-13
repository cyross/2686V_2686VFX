#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"

class Wt2Processor : public PrBase
{
    std::atomic<float>* pBit = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pLevel = nullptr;
    std::atomic<float>* pSampleSize = nullptr;
    std::atomic<float>* pResolution = nullptr;
    std::atomic<float>* pWave = nullptr;
    std::array<std::atomic<float>*, 32> pCustom32 = { nullptr };
    std::array<std::atomic<float>*, 64> pCustom64 = { nullptr };
    std::array<std::atomic<float>*, 128>  pCustom128 = { nullptr };
    std::array<std::atomic<float>*, 256>  pCustom256 = { nullptr };
    std::atomic<float>* pFixMode = nullptr;
    std::atomic<float>* pFixFreq = nullptr;

    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;

    std::atomic<float>* pAdsrBypass = nullptr;
    std::atomic<float>* pAdsrStl = nullptr;
    std::atomic<float>* pAdsrAr = nullptr;
    std::atomic<float>* pAdsrDr = nullptr;
    std::atomic<float>* pAdsrSl = nullptr;
    std::atomic<float>* pAdsrRr = nullptr;
    std::atomic<float>* pAdsrKor = nullptr;

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
    std::atomic<float>* pDetune3 = nullptr;

    std::atomic<float>* pModEnable = nullptr;
    std::atomic<float>* pModDepth = nullptr;
    std::atomic<float>* pModSpeed = nullptr;

    std::atomic<float>* pLfoPmSyncDelay = nullptr;
    std::atomic<float>* pLfoAmSyncDelay = nullptr;
    std::atomic<float>* pLfoAmSmoothRatio = nullptr;
    std::atomic<float>* pLfoPmFreq = nullptr;
    std::atomic<float>* pLfoAmFreq = nullptr;
    std::atomic<float>* pLfoPmShape = nullptr;
    std::atomic<float>* pLfoAmShape = nullptr;
    std::atomic<float>* pLfoPm = nullptr;
    std::atomic<float>* pLfoAm = nullptr;
    std::atomic<float>* pLfoPmd = nullptr;
    std::atomic<float>* pLfoPms = nullptr;
    std::atomic<float>* pLfoAmd = nullptr;
    std::atomic<float>* pLfoAms = nullptr;
public:
    void createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& code, const juce::String& name);
    template <size_t I>
    void processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& code);
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
