#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "Shared/Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "Shared/Core/Processor/ProcessorValues.h"

class RhythmProcessor : public PrBase
{
    PrPtrsRhythmBasic pBasic;
    PrPtrsUnison pUnison;

    std::array<PrPtrsRhythmPadBasic, RhythmPrValue::maxPads> pPadBasic;
    std::array<PrPtrsQualityPcm, RhythmPrValue::maxPads> pQuality;
    std::array<PrPtrsAdsrAmpEnv, RhythmPrValue::maxPads> pAmpEnv;
    std::array<PrPtrsWtMod, RhythmPrValue::maxPads> pWtMod;
    std::array<PrPtrsWtAmpMod, RhythmPrValue::maxPads> pWtAmpMod;
    std::array<PrPtrsPitchEnv, RhythmPrValue::maxPads> pPitchEnv;
    std::array<PrPtrsSsgSwEnv, RhythmPrValue::maxPads> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11, RhythmPrValue::maxPads> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11, RhythmPrValue::maxPads> pSsgSwPEnv11;
    std::array<PrPtrsOpzx7Detune, RhythmPrValue::maxPads> pOpzx7Detune;
    std::array<PrPtrsFix, RhythmPrValue::maxPads> pFix;
    std::array<PrPtrsOpzx7Lfo, RhythmPrValue::maxPads> pOpzx7Lfo;
    std::array<PrPtrsToneNoise, RhythmPrValue::maxPads> pToneNoise;
    std::array<PrPtrsPcm, RhythmPrValue::maxPads> pPcm;
    std::array<PrPtrsLp, RhythmPrValue::maxPads> pLp;
    std::array<PrPtrsSsgHwEnv, RhythmPrValue::maxPads> pSsgHwEnv;
    std::array<PrPtrsSsgHwPEnv, RhythmPrValue::maxPads> pSsgHwPEnv;

    // パッドの数 (86V は 6) と QUALITY(PCM) の BIT の初期値。
    // プラグインが作るときに決める。入れ物は最大数 (maxPads) ぶん持つ。
    int m_pads;
    int m_pcmBitInitial;
public:
    explicit RhythmProcessor(int pads = RhythmPrValue::maxPads, int pcmBitInitial = CPV::QualityPcm::Bit::initial)
        : m_pads(juce::jlimit(1, RhythmPrValue::maxPads, pads)), m_pcmBitInitial(pcmBitInitial) {}

    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves);
};
