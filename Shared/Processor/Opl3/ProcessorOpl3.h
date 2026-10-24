#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "Shared/Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Core/Processor/ProcessorTapSeq.h"
#include "../../Core/Processor/ProcessorParamMod.h"
#include "../../Core/Processor/ProcessorValueEnv.h"

class Opl3Processor : public PrBase, public ParamMod::Applier
{
    PrPtrsOpl3Basic pBasic;
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

    std::array<PrPtrsOplDetune, Opl3PrValue::ops> pOpDetune;
    std::array<PrPtrsOpl3Adsr, Opl3PrValue::ops> pOpAdsr;
    std::array<PrPtrsOplEg, Opl3PrValue::ops> pOpEg;
    std::array<PrPtrsOplLfo, Opl3PrValue::ops> pLfo;
    std::array<PrPtrsPitchEnvOp, Opl3PrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnv256Op, Opl3PrValue::ops> pSsgSwEnv256;
    // PARAM ENV[256] (オペレーター)。4.0.0 の TL ENV の代わり
    std::array<ParamEnv::Ptrs, Opl3PrValue::ops> pOpParamEnv;

    // ALG ENV (チャンネル)
    ValueEnv::Ptrs pAlgEnv;
    std::array<PrPtrsSsgSwPEnv256Op, Opl3PrValue::ops> pSsgSwPEnv256;
    std::array<PrPtrsSsgHwPEnv, Opl3PrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, Opl3PrValue::ops> pOpWtAmpMod;
    std::array<PrPtrsSsgHwEnv, Opl3PrValue::ops> pOpSsgHwEnv;
    std::array<PrPtrsWtMod, Opl3PrValue::ops> pOpWtMod;
    // 押してから鳴り始めるまでの間 (秒)。オペレーター 1 本ごと。
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpDelay = { nullptr };

    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpMask = { nullptr };
    // TAP AMP / PITCH SEQUENCER
    TapSeq::Ptrs pTapAmp;
    TapSeq::Ptrs pTapPitch;

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapSeq::Ptrs pTapParam;

    // TAP PARAM SEQUENCER (オペレーターごと)
    std::array<TapSeq::Ptrs, Opl3PrValue::ops> pOpTapParam;
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
    void applyChannel(Opl3Params& p);
    void applyOp(Opl3Params& p, int op);
};
