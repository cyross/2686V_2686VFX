#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Core/Processor/ProcessorStructs.h"

class OpmProcessor : public PrBase
{
    PrPtrsOpmBasic pBasic;
    PrPtrsAlgFb pAlgFb;
    PrPtrsQuality pQuality;
    PrPtrsOpmLfo pGlLfo;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsSsgHwPEnv pSsgHwPEnv;
    PrPtrsAdsrAmpEnv pAmpEnvG;
    PrPtrsWtMod pWtMod;
    PrPtrsWtAmpMod pWtAmpMod;
    PrPtrsSsgSwEnv11 pSsgSwEnv11g;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11g;
    PrPtrsUnison pUnison;

    std::array<PrPtrsOpmDetune, OpmPrValue::ops> pOpDetune;
    std::array<PrPtrsOpmAdsr, OpmPrValue::ops> pOpAdsr;
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpLfoAmsEnable = { nullptr };
    std::array<PrPtrsFix, OpmPrValue::ops> pFix;
    std::array<PrPtrsPitchEnvOp, OpmPrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnvOp, OpmPrValue::ops> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11Op, OpmPrValue::ops> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11Op, OpmPrValue::ops> pSsgSwPEnv11;
    std::array<PrPtrsSsgHwPEnv, OpmPrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, OpmPrValue::ops> pOpWtAmpMod;
    std::array<PrPtrsSsgHwEnv, OpmPrValue::ops> pOpSsgHwEnv;
    std::array<PrPtrsWtMod, OpmPrValue::ops> pOpWtMod;
    std::array<std::atomic<float>*, OpmPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves);
};
