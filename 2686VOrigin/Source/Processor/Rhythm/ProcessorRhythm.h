#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Core/Processor/ProcessorStructs.h"

class RhythmProcessor : public PrBase
{
    PrPtrsRhythmBasic pBasic;
    PrPtrsUnison pUnison;

    std::array<PrPtrsRhythmPadBasic, RhythmPrValue::pads> pPadBasic;
    std::array<PrPtrsQualityPcm, RhythmPrValue::pads> pQuality;
    std::array<PrPtrsAdsrAmpEnv, RhythmPrValue::pads> pAmpEnv;
    std::array<PrPtrsPitchEnv, RhythmPrValue::pads> pPitchEnv;
    std::array<PrPtrsSsgSwEnv, RhythmPrValue::pads> pSsgSwEnv;
    std::array<PrPtrsSsgSwEnv11, RhythmPrValue::pads> pSsgSwEnv11;
    std::array<PrPtrsSsgSwPEnv11, RhythmPrValue::pads> pSsgSwPEnv11;
    std::array<PrPtrsOpzx7Detune, RhythmPrValue::pads> pOpzx7Detune;
    std::array<PrPtrsFix, RhythmPrValue::pads> pFix;
    std::array<PrPtrsOpzx7Lfo, RhythmPrValue::pads> pOpzx7Lfo;
    std::array<PrPtrsToneNoise, RhythmPrValue::pads> pToneNoise;
    std::array<PrPtrsPcm, RhythmPrValue::pads> pPcm;
    std::array<PrPtrsLp, RhythmPrValue::pads> pLp;
    std::array<PrPtrsSsgHwEnv, RhythmPrValue::pads> pSsgHwEnv;
public:
    void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) override;
    void processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts) override;
    void init(juce::AudioProcessorValueTreeState& apvts);
};
