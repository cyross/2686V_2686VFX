#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"

class RhythmProcessor : public PrBase
{
    std::atomic<float>* pMLevel = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, 8> pLevel = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPan = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pNoteNumber = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pQualityMode = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pRateIndex = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pIsOneShot = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPcmOffset = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPcmRatio = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrBypass = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrStl = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrAr = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrDr = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrSl = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrRr = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pAdsrKor = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrBypass = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrAr = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrDr = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrRr = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrStl = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrAtl = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrSsl = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 8> pPitchAdsrRll = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
