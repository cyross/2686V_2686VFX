#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"

class OpnaProcessor : public PrBase
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
    std::atomic<float>* pPan = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetuneCents = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;

    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpMultiple = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpDetune = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrRgAr = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrRgD1r = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrRgD1l = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrRgD2r = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrRgRr = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrRgTl = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrKs = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrXof = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpAdsrKor = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgEg = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgEgFreq = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpFixEnable = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpFixFreq = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpLfoFreqs = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpLfoSyncDelay = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpLfoPmEnable = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpLfoPms = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpLfoAmEnable = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpLfoAms = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpN88LfoAms = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvAr = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvDr = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvRr = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvStl = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvAtl = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvSsl = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpPitchEnvRll = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvEnable = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpSsgSwEnvL6 = { nullptr };
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
