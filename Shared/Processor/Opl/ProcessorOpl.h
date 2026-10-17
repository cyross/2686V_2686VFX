#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "Shared/Processor/Opl/ProcessorOplValues.h"
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
    PrPtrsSsgSwEnv256 pSsgSwEnv256g;
    PrPtrsSsgSwPEnv256 pSsgSwPEnv256g;
    PrPtrsUnison pUnison;

    std::array<PrPtrsOplDetune, OplPrValue::ops> pOpDetune;
    std::array<PrPtrsOplAdsr, OplPrValue::ops> pOpAdsr;
    std::array<PrPtrsOplEg, OplPrValue::ops> pOpEg;
    std::array<PrPtrsOplLfo, OplPrValue::ops> pLfo;
    std::array<PrPtrsPitchEnvOp, OplPrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnv256Op, OplPrValue::ops> pSsgSwEnv256;
    std::array<PrPtrsSsgSwPEnv256Op, OplPrValue::ops> pSsgSwPEnv256;
    std::array<PrPtrsSsgHwPEnv, OplPrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, OplPrValue::ops> pOpWtAmpMod;
    std::array<PrPtrsSsgHwEnv, OplPrValue::ops> pOpSsgHwEnv;
    std::array<PrPtrsWtMod, OplPrValue::ops> pOpWtMod;
    // 押してから鳴り始めるまでの間 (秒)。オペレーター 1 本ごと。
    std::array<std::atomic<float>*, OplPrValue::ops> pOpDelay = { nullptr };

    std::array<std::atomic<float>*, OplPrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables);
};
