#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "../../Core/Processor/ProcessorStructs.h"

class OpnaProcessor : public PrBase
{
    PrPtrsOpnaBasic pBasic;
    PrPtrsAlgFb pAlgFb;
    PrPtrsQuality pQuality;
    PrPtrsN88Lfo pN88Lfo;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsAdsrAmpEnv pAmpEnvG;
    PrPtrsSsgSwEnv11 pSsgSwEnv11g;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11g;
    PrPtrsUnison pUnison;

    std::array<PrPtrsOpnDetune, OpnaPrValue::ops> pOpDetune;
    std::array<PrPtrsOpnAdsr, OpnaPrValue::ops> pOpAdsr;
    std::array<PrPtrsSsgEg, OpnaPrValue::ops> pOpSsgEg;
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpN88LfoAms = { nullptr };
    std::array<PrPtrsOpnaLfo, OpnaPrValue::ops> pLfo;
    std::array<PrPtrsFix, OpnaPrValue::ops> pFix;
    std::array<PrPtrsPitchEnvOp, OpnaPrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnvOp, OpnaPrValue::ops> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11Op, OpnaPrValue::ops> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11Op, OpnaPrValue::ops> pSsgSwPEnv11;
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
