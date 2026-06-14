#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"

class Opzx7Processor : public PrBase
{
    std::atomic<float>* pLevel = nullptr;
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pLfoPmFreq = nullptr;
    std::atomic<float>* pLfoAmFreq = nullptr;
    std::atomic<float>* pLfoPmSyncDelay = nullptr;
    std::atomic<float>* pLfoAmSyncDelay = nullptr;
    std::atomic<float>* pLfoPmShape = nullptr;
    std::atomic<float>* pLfoAmShape = nullptr;
    std::atomic<float>* pLfoPm = nullptr;
    std::atomic<float>* pLfoPmd = nullptr;
    std::atomic<float>* pLfoPms = nullptr;
    std::atomic<float>* pLfoAm = nullptr;
	std::atomic<float>* pLfoAmSmoothRatio = nullptr;
    std::atomic<float>* pLfoAmd = nullptr;
    std::atomic<float>* pLfoAms = nullptr;
    std::atomic<float>* pPanpotEnable = nullptr;
    std::atomic<float>* pPanpot = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, 4> pOpMultiple = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpMultipleRatio = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpDetune = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpDetune2 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpDetune3 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrBypass = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRealAr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRealD1r = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRealD1l = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRealD2r = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRealRr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRealTl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgAr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgD1r = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgD1l = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgD2r = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgRr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgTl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKsr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKsl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKsEn = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrSus = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrXof = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKor = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgEg = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpSsgEgFreq = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpFixEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpFixFreq = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpWaveSelect = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPcmOffset = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpPcmRatio = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPmFreq = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmFreq = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPmSyncDelay = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmSyncDelay = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPm = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPmShape = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPms = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoPmd = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAm = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmShape = { nullptr, nullptr,nullptr,nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAms = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmd = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmSmoothRatio = { nullptr, nullptr, nullptr, nullptr };
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
