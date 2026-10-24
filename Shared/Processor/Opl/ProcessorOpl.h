#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "Shared/Processor/Opl/ProcessorOplValues.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Core/Processor/ProcessorTapSeq.h"
#include "../../Core/Processor/ProcessorParamMod.h"
#include "../../Core/Processor/ProcessorValueEnv.h"

class OplProcessor : public PrBase, public ParamMod::Applier
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
    // PARAM ENV[256] (オペレーター)。4.0.0 の TL ENV の代わり
    std::array<ParamEnv::Ptrs, OplPrValue::ops> pOpParamEnv;

    // ALG ENV (チャンネル)
    ValueEnv::Ptrs pAlgEnv;
    std::array<PrPtrsSsgSwPEnv256Op, OplPrValue::ops> pSsgSwPEnv256;
    std::array<PrPtrsSsgHwPEnv, OplPrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, OplPrValue::ops> pOpWtAmpMod;
    std::array<PrPtrsSsgHwEnv, OplPrValue::ops> pOpSsgHwEnv;
    std::array<PrPtrsWtMod, OplPrValue::ops> pOpWtMod;
    // 押してから鳴り始めるまでの間 (秒)。オペレーター 1 本ごと。
    std::array<std::atomic<float>*, OplPrValue::ops> pOpDelay = { nullptr };

    std::array<std::atomic<float>*, OplPrValue::ops> pOpMask = { nullptr };
    // TAP AMP / PITCH SEQUENCER
    TapSeq::Ptrs pTapAmp;
    TapSeq::Ptrs pTapPitch;

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapSeq::Ptrs pTapParam;
    ParamEnv::Ptrs pParamEnv;

    // PARAM ENV / TAP PARAM SEQUENCER の指し先
    ParamModResolver modResolver;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables);

    // PARAM ENV / TAP PARAM SEQUENCER (4.3.0)。ボイスが置き換えた値で読み直す
    void apply(void* chip, const ParamMod::Override* list, int count) override;

    const ParamModResolver& paramMod() const { return modResolver; }
private:
    void applyChannel(OplParams& p);
    void applyOp(OplParams& p, int op);
};
