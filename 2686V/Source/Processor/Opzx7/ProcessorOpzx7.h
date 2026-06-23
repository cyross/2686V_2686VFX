#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"

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
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpMultiple = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpMultipleRatio = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpDetune = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpDetune2 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpDetune3 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrBypass = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgEnable = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRealAr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRealD1r = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRealD1l = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRealD2r = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRealRr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRealTl = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgAr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgD1r = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgD1l = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgD2r = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgRr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrRgTl = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsEn = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsMode = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsrMA7 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKslMA7 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsrOPZ = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKslOPZ = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsBp = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsLc = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsRc = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsLd = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsRd = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKsRs = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrSus = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrXof = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpAdsrKor = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgEg = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgEgFreq = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpFixEnable = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpFixFreq = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpWaveSelect = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPcmOffset = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPcmRatio = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pLoopPointEnable = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pLoopPointStart = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pLoopPointEnd = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoPmFreq = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAmFreq = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoPmSyncDelay = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAmSyncDelay = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoPm = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoPmShape = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoPms = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoPmd = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAm = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAmShape = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAms = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAmd = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpLfoAmSmoothRatio = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvEnable = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvAr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvDr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvRr = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvStl = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvAtl = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvSsl = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpPitchEnvRll = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvEnable = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvSteps = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvLoop = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvLoopTo = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvLoopCount = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvStl = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvR1 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvL1 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvR2 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvL2 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvR3 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvL3 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvR4 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvL4 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvR5 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvL5 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvR6 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpSsgSwEnvL6 = { nullptr };
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
