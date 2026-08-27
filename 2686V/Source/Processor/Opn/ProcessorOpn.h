#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Core/Processor/ProcessorStructs.h"

class OpnProcessor : public PrBase
{
    PrPtrsOpnBasic pBasic;
    PrPtrsAlgFb pAlgFb;
    PrPtrsQuality pQuality;
    PrPtrsN88Lfo pN88Lfo;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsAdsrAmpEnv pAmpEnvG;
    PrPtrsWtMod pWtMod;
    PrPtrsSsgSwEnv11 pSsgSwEnv11g;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11g;
    PrPtrsUnison pUnison;

    std::array<PrPtrsOpnDetune, OpnPrValue::ops> pOpDetune;
    std::array<PrPtrsOpnAdsr, OpnPrValue::ops> pOpAdsr;
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpN88LfoAms = { nullptr };
    std::array<PrPtrsFix, OpnPrValue::ops> pFix;
    std::array<PrPtrsPitchEnvOp, OpnPrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnvOp, OpnPrValue::ops> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11Op, OpnPrValue::ops> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11Op, OpnPrValue::ops> pSsgSwPEnv11;
    std::array<std::atomic<float>*, OpnPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
