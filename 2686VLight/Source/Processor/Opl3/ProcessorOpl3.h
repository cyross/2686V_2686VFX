#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Core/Processor/ProcessorStructs.h"

class Opl3Processor : public PrBase
{
    PrPtrsOpl3Basic pBasic;
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

    std::array<PrPtrsOplDetune, Opl3PrValue::ops> pOpDetune;
    std::array<PrPtrsOpl3Adsr, Opl3PrValue::ops> pOpAdsr;
    std::array<PrPtrsOplEg, Opl3PrValue::ops> pOpEg;
    std::array<PrPtrsOplLfo, Opl3PrValue::ops> pLfo;
    std::array<PrPtrsPitchEnvOp, Opl3PrValue::ops> pPitchEnv;
    std::array<PrPtrsSsgSwEnvOp, Opl3PrValue::ops> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11Op, Opl3PrValue::ops> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11Op, Opl3PrValue::ops> pSsgSwPEnv11;
    std::array<PrPtrsSsgHwPEnv, Opl3PrValue::ops> pOpSsgHwPEnv;
    std::array<PrPtrsWtAmpMod, Opl3PrValue::ops> pOpWtAmpMod;
    std::array<PrPtrsSsgHwEnv, Opl3PrValue::ops> pOpSsgHwEnv;
    std::array<PrPtrsWtMod, Opl3PrValue::ops> pOpWtMod;
    std::array<std::atomic<float>*, Opl3PrValue::ops> pOpMask = { nullptr };
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves);
};
