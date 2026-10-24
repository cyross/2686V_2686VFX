#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Core/Processor/ProcessorTapSeq.h"
#include "../../Core/Processor/ProcessorParamMod.h"
#include "../../Core/Processor/ProcessorValueEnv.h"

class SsgProcessor : public PrBase, public ParamMod::Applier
{
    PrPtrsSsgBasic pBasic;
    PrPtrsQuality pQuality;
    PrPtrsAdsrAmpEnv pAmpEnv;
    PrPtrsWtMod pWtMod;
    PrPtrsWtAmpMod pWtAmpMod;
    PrPtrsPitchEnv pPitchEnv;
    PrPtrsSsgSwEnv256 pSsgSwEnv256;
    PrPtrsSsgSwPEnv256 pSsgSwPEnv256;
    PrPtrsOpzx7Detune pOpzx7Detune;
    PrPtrsOpzx7Lfo pOpzx7Lfo;
    PrPtrsFix pFix;
    PrPtrsToneNoise pToneNoise;
    PrPtrsSsgDuty pDuty;
    PrPtrsSsgTri pTri;
    ValueEnv::Ptrs pDutyEnv;
    ValueEnv::Ptrs pPeakEnv;
    PrPtrsSsgHwEnv pHwEnv;
    PrPtrsSsgHwPEnv pHwPEnv;
    PrPtrsUnison pUnison;
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
    void applyChannel(SsgParams& p);
};
