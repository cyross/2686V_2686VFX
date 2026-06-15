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
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
