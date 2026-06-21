#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"

class RhythmProcessor : public PrBase
{
    std::atomic<float>* pMLevel = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLevel = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPan = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pNoteNumber = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pQualityMode = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pRateIndex = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pInterpolationMode = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pTone = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pNoise = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pNoiseFreq = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pMix = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pFixMode = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pFixFreq = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pIsOneShot = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPcmOffset = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPcmRatio = { nullptr };

    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrStl = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrAr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrDr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrSl = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrRr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pAdsrKor = { nullptr };

    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrAr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrDr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrRr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrStl = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrAtl = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrSsl = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pPitchAdsrRll = { nullptr };

    std::array<std::atomic<float>*, RhythmPrValue::pads> pMultiple = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pMultipleRatio = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pDetune = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pDetune2 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pDetune3 = { nullptr };

    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvBypass = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvDr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvRr = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pSsgSwEnvL6 = { nullptr };

    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoPmSyncDelay = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAmSyncDelay = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAmSmoothRatio = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoPmFreq = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAmFreq = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoPmShape = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAmShape = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoPm = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAm = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoPmd = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoPms = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAmd = { nullptr };
    std::array<std::atomic<float>*, RhythmPrValue::pads> pLfoAms = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
