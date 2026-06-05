#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/Base.h"

class OpmProcessor : public PrBase
{
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pLfoFreq = nullptr;
    std::atomic<float>* pLfoSyncDelay = nullptr;
    std::atomic<float>* pLfoPmWave = nullptr;
    std::atomic<float>* pLfoAmWave = nullptr;
    std::atomic<float>* pLfoPmEnable = nullptr;
    std::atomic<float>* pLfoPmd = nullptr;
    std::atomic<float>* pLfoPms = nullptr;
    std::atomic<float>* pLfoAmEnable = nullptr;
    std::atomic<float>* pLfoAmSmRt = nullptr;
    std::atomic<float>* pLfoAmd = nullptr;
    std::atomic<float>* pLfoAms = nullptr;
    std::atomic<float>* pPan = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::array<std::atomic<float>*, 4> pOpMultiple = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpDetune = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpDetune2 = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrBypass = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgAr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgD1r = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgD1l = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgD2r = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgRr = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrRgTl = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKs = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrXof = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpAdsrKor = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpFixEnable = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpFixFreq = { nullptr, nullptr, nullptr, nullptr };
    std::array<std::atomic<float>*, 4> pOpLfoAmsEnable = { nullptr, nullptr, nullptr, nullptr };
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
