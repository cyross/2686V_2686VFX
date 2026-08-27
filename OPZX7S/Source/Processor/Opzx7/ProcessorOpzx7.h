#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Core/Processor/ProcessorStructs.h"

class Opzx7Processor : public PrBase
{
    PrPtrsOpzx7Basic pBasic;
    PrPtrsOpzx7AlgFb pAlgFb;
    PrPtrsQuality pQuality;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsAdsrAmpEnv pAmpEnvG;
    PrPtrsWtMod pWtMod;
    PrPtrsSsgSwEnv11 pSsgSwEnv11g;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11g;
    PrPtrsOpzx7Lfo pOpzx7Lfo;
    PrPtrsPanpot pPanpot;
    PrPtrsUnison pUnison;
    std::array<PrPtrsOpzx7Detune, Opzx7PrValue::ops> pOpzx7Detune;
    std::array<PrPtrsOpzx7Adsr, Opzx7PrValue::ops> pOpAdsr;
    std::array<PrPtrsSsgEg, Opzx7PrValue::ops> pOpSsgEg;
    std::array<PrPtrsWs, Opzx7PrValue::ops> pOpWs;
    std::array<PrPtrsOpzx7Lfo, Opzx7PrValue::ops> pOpOpzx7Lfo;
    std::array<PrPtrsFix, Opzx7PrValue::ops> pFix;
    std::array<PrPtrsPcm, Opzx7PrValue::ops> pPcm;
    std::array<PrPtrsLp, Opzx7PrValue::ops> pLp;
    std::array<PrPtrsPitchEnvOp, Opzx7PrValue::ops> pOpPitchEnv;
    std::array<PrPtrsSsgSwEnvOp, Opzx7PrValue::ops> pOpSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11Op, Opzx7PrValue::ops> pOpSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11Op, Opzx7PrValue::ops> pOpSsgSwPEnv11;
    std::array<std::atomic<float>*, Opzx7PrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
