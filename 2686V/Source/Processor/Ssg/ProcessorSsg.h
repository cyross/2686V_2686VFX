#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"

class SsgProcessor : public PrBase
{
    std::atomic<float>* pTone = nullptr;
    std::atomic<float>* pNoise = nullptr;
    std::atomic<float>* pNoiseFreq = nullptr;
    std::atomic<float>* pNoiseOnNote = nullptr;
    std::atomic<float>* pMix = nullptr;
    std::atomic<float>* pWaveform = nullptr;
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

    std::atomic<float>* pDutyMode = nullptr;
    std::atomic<float>* pDutyPreset = nullptr;
    std::atomic<float>* pDutyVar = nullptr;
    std::atomic<float>* pDutyInvert = nullptr;
    std::atomic<float>* pDutyFc = nullptr;
    std::atomic<float>* pDutyFcFluc = nullptr;
    std::atomic<float>* pTriKeyTrk = nullptr;
    std::atomic<float>* pTriPeak = nullptr;
    std::atomic<float>* pTriFreq = nullptr;
    std::atomic<float>* pBit = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pHwEnvEnable = nullptr;
    std::atomic<float>* pHwEnvShape = nullptr;
    std::atomic<float>* pHwEnvPeriod = nullptr;

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
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
