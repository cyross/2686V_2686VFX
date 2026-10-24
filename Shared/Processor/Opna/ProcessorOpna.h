#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "Shared/Processor/Opna/ProcessorOpnaValues.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Core/Processor/ProcessorTapSeq.h"
#include "../../Core/Processor/ProcessorParamMod.h"
#include "../../Core/Processor/ProcessorValueEnv.h"

class OpnaProcessor : public PrBase, public ParamMod::Applier
{
    PrPtrsOpnaBasic pBasic;
    PrPtrsAlgFb pAlgFb;
    PrPtrsQuality pQuality;
    PrPtrsN88Lfo pN88Lfo;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsSsgHwPEnv pSsgHwPEnv;
    PrPtrsAdsrAmpEnv pAmpEnvG;
    PrPtrsWtMod pWtMod;
    PrPtrsWtAmpMod pWtAmpMod;
    PrPtrsSsgSwEnv256 pSsgSwEnv256g;
    PrPtrsSsgSwPEnv256 pSsgSwPEnv256g;
    PrPtrsUnison pUnison;

    std::array<PrPtrsOpnDetune, OpnaPrValue::ops> pOpDetune;
    std::array<PrPtrsOpnAdsr, OpnaPrValue::ops> pOpAdsr;
    std::array<PrPtrsSsgEg, OpnaPrValue::ops> pOpSsgEg;
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpN88LfoAms = { nullptr };
    std::array<PrPtrsOpnaLfo, OpnaPrValue::ops> pLfo;
    std::array<PrPtrsFix, OpnaPrValue::ops> pFix;
    std::array<PrPtrsPitchEnvOp, OpnaPrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnv256Op, OpnaPrValue::ops> pSsgSwEnv256;
    // PARAM ENV[256] (オペレーター)。4.0.0 の TL ENV の代わり
    std::array<ParamEnv::Ptrs, OpnaPrValue::ops> pOpParamEnv;

    // ALG ENV (チャンネル)
    ValueEnv::Ptrs pAlgEnv;
    std::array<PrPtrsSsgSwPEnv256Op, OpnaPrValue::ops> pSsgSwPEnv256;
    std::array<PrPtrsSsgHwPEnv, OpnaPrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, OpnaPrValue::ops> pOpWtAmpMod;
    std::array<PrPtrsSsgHwEnv, OpnaPrValue::ops> pOpSsgHwEnv;
    std::array<PrPtrsWtMod, OpnaPrValue::ops> pOpWtMod;
    // 押してから鳴り始めるまでの間 (秒)。オペレーター 1 本ごと。
    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpDelay = { nullptr };

    std::array<std::atomic<float>*, OpnaPrValue::ops> pOpMask = { nullptr };
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
    void applyChannel(OpnaParams& p);
    void applyOp(OpnaParams& p, int op);
};
