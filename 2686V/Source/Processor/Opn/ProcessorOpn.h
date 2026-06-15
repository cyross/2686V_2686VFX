#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"

class OpnProcessor : public PrBase
{
    std::atomic<float>* pLevel = nullptr;
    std::atomic<float>* pAlg = nullptr;
    std::atomic<float>* pFb = nullptr;
    std::atomic<float>* pDepth = nullptr;
    std::atomic<float>* pRate = nullptr;
    std::atomic<float>* pN88LfoFreq = nullptr;
    std::atomic<float>* pN88LfoSyncDelay = nullptr;
    std::atomic<float>* pN88LfoShape = nullptr;
    std::atomic<float>* pN88LfoPmEnable = nullptr;
    std::atomic<float>* pN88LfoPmd = nullptr;
    std::atomic<float>* pN88LfoPms = nullptr;
    std::atomic<float>* pN88LfoAmEnable = nullptr;
    std::atomic<float>* pN88LfoAmSmRt = nullptr;
    std::atomic<float>* pN88LfoAmd = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;

    std::array<std::atomic<float>*, OpnPrValue::ops> pOpMultiple = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpDetune = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrRgAr = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrRgD1r = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrRgD1l = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrRgD2r = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrRgRr = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrRgTl = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrKs = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrXof = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpAdsrKor = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpFixEnable = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpFixFreq = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpN88LfoAms = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvAr = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvDr = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvRr = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvStl = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvAtl = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvSsl = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpPitchEnvRll = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpSsgSwEnvL6 = { nullptr };
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
