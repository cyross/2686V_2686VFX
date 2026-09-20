#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "Shared/Core/Processor/ProcessorValues.h"

class AdpcmProcessor : public PrBase
{
    PrPtrsAdpcmBasic pBasic;
    PrPtrsQualityPcm pQuality;
    PrPtrsAdsrAmpEnv pAmpEnv;
    PrPtrsWtMod pWtMod;
    PrPtrsWtAmpMod pWtAmpMod;
    PrPtrsPitchEnv pPitchEnv;
    PrPtrsSsgSwEnv pSsgSwEnv;
    PrPtrsSsgSwEnv11 pSsgSwEnv11;
    PrPtrsSsgSwPEnv11 pSsgSwPEnv11;
    PrPtrsSsgHwEnv pSsgHwEnv;
    PrPtrsSsgHwPEnv pSsgHwPEnv;
    PrPtrsOpzx7Detune pOpzx7Detune;
    PrPtrsOpzx7Lfo pOpzx7Lfo;
    PrPtrsFix pFix;
    PrPtrsToneNoise pToneNoise;
    PrPtrsPcm pPcm;
    PrPtrsLp pLp;
    PrPtrsUnison pUnison;

    // QUALITY(PCM) の BIT の初期値。プラグインが作るときに決める。
    int m_pcmBitInitial;
public:
    explicit AdpcmProcessor(int pcmBitInitial = CPV::QualityPcm::Bit::initial) : m_pcmBitInitial(pcmBitInitial) {}

    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves);
};
