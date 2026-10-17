#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Core/Processor/ProcessorTapSeq.h"

class BeepProcessor : public PrBase {
    PrPtrsBeepBasic pBasic;
    PrPtrsAdsrAmpEnv pAmpEnv;
    PrPtrsWtMod pWtMod;
    PrPtrsWtAmpMod pWtAmpMod;
    PrPtrsPitchEnv pPitchEnv;
    PrPtrsSsgSwEnv256 pSsgSwEnv256;
    PrPtrsSsgSwPEnv256 pSsgSwPEnv256;
    PrPtrsOpzx7Detune pOpzx7Detune;
    PrPtrsOpzx7Lfo pOpzx7Lfo;
    PrPtrsFix pFix;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsSsgHwPEnv pSsgHwPEnv;
    PrPtrsUnison pUnison;
    // TAP AMP / PITCH SEQUENCER
    TapSeq::Ptrs pTapAmp;
    TapSeq::Ptrs pTapPitch;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables);
};