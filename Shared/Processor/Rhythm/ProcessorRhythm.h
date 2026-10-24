#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Synth/SynthCoreParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "Shared/Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Core/Processor/ProcessorTapSeq.h"
#include "../../Core/Processor/ProcessorParamMod.h"
#include "Shared/Core/Processor/ProcessorValues.h"

class RhythmProcessor : public PrBase, public ParamMod::Applier
{
    PrPtrsRhythmBasic pBasic;
    PrPtrsUnison pUnison;

    std::array<PrPtrsRhythmPadBasic, RhythmPrValue::maxPads> pPadBasic;
    std::array<PrPtrsQualityPcm, RhythmPrValue::maxPads> pQuality;
    std::array<PrPtrsAdsrAmpEnv, RhythmPrValue::maxPads> pAmpEnv;
    std::array<PrPtrsWtMod, RhythmPrValue::maxPads> pWtMod;
    std::array<PrPtrsWtAmpMod, RhythmPrValue::maxPads> pWtAmpMod;
    std::array<PrPtrsPitchEnv, RhythmPrValue::maxPads> pPitchEnv;
    std::array<PrPtrsSsgSwEnv256, RhythmPrValue::maxPads> pSsgSwEnv256;
    std::array<PrPtrsSsgSwPEnv256, RhythmPrValue::maxPads> pSsgSwPEnv256;
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
    // TAP AMP / PITCH SEQUENCER
    TapSeq::Ptrs pTapAmp;
    TapSeq::Ptrs pTapPitch;

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapSeq::Ptrs pTapParam;

    // TAP PARAM SEQUENCER (パッドごと)
    std::array<TapSeq::Ptrs, RhythmPrValue::maxPads> pOpTapParam;
    ParamEnv::Ptrs pParamEnv;

    // PARAM ENV / TAP PARAM SEQUENCER の指し先
    ParamModResolver modResolver;
public:
    explicit RhythmProcessor(int pads = RhythmPrValue::maxPads, int pcmBitInitial = CPV::QualityPcm::Bit::initial)
        : m_pads(juce::jlimit(1, RhythmPrValue::maxPads, pads)), m_pcmBitInitial(pcmBitInitial) {}

    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    // modWaves は WT PITCH MOD の変調波形の置き場所。
    // パラメータではなくプロセッサが持つので、ここで受け取る。
    void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables);

    // PARAM ENV / TAP PARAM SEQUENCER (4.3.0)。ボイスが置き換えた値で読み直す
    void apply(void* chip, const ParamMod::Override* list, int count) override;

    const ParamModResolver& paramMod() const { return modResolver; }
private:
    void applyChannel(RhythmParams& p);
    void applyOp(RhythmParams& p, int i);
};
