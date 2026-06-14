#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"

class OpmProcessor : public PrBase
{
    std::atomic<float>* pLevel = nullptr;
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
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpMultiple = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpDetune = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpDetune2 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrRgAr = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrRgD1r = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrRgD1l = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrRgD2r = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrRgRr = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrRgTl = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrKs = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrXof = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpAdsrKor = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpFixEnable = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpFixFreq = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpLfoAmsEnable = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvAr = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvDr = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvRr = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvStl = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvAtl = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvSsl = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpPitchEnvRll = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpSsgSwEnvL6 = { nullptr };
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
