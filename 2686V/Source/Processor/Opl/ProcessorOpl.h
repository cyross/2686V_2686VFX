#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Core/Processor/ProcessorStructs.h"

class OplProcessor : public PrBase
{
    PrPtrsOplBasic pBasic;
    PrPtrsAlgFb pAlgFb;
    PrPtrsQuality pQuality;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsSsgHwPEnv pSsgHwPEnv;
    PrPtrsAdsrAmpEnv pAmpEnvG;
    PrPtrsWtMod pWtMod;
    PrPtrsWtAmpMod pWtAmpMod;
    PrPtrsSsgSwEnv11 pSsgSwEnv11g;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11g;
    PrPtrsUnison pUnison;

    std::array<PrPtrsOplDetune, OplPrValue::ops> pOpDetune;
    std::array<PrPtrsOplAdsr, OplPrValue::ops> pOpAdsr;
    std::array<PrPtrsOplEg, OplPrValue::ops> pOpEg;
    std::array<PrPtrsOplLfo, OplPrValue::ops> pLfo;
    std::array<PrPtrsPitchEnvOp, OplPrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnvOp, OplPrValue::ops> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11Op, OplPrValue::ops> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11Op, OplPrValue::ops> pSsgSwPEnv11;
    std::array<PrPtrsSsgHwPEnv, OplPrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, OplPrValue::ops> pOpWtAmpMod;
    std::array<std::atomic<float>*, OplPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves);
};
