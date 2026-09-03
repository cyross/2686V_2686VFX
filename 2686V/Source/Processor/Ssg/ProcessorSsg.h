#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Core/Processor/ProcessorStructs.h"

class SsgProcessor : public PrBase
{
    PrPtrsSsgBasic pBasic;
    PrPtrsQuality pQuality;
    PrPtrsAdsrAmpEnv pAmpEnv;
    PrPtrsWtMod pWtMod;
    PrPtrsPitchEnv pPitchEnv;
    PrPtrsSsgSwEnv pSsgSwEnv;
    PrPtrsSsgSwEnv11 pSsgSwEnv11;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11;
    PrPtrsOpzx7Detune pOpzx7Detune;
    PrPtrsOpzx7Lfo pOpzx7Lfo;
    PrPtrsFix pFix;
    PrPtrsToneNoise pToneNoise;
    PrPtrsSsgDuty pDuty;
    PrPtrsSsgTri pTri;
    PrPtrsSsgHwEnv pHwEnv;
    PrPtrsSsgHwPEnv pHwPEnv;
    PrPtrsUnison pUnison;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves);
};
