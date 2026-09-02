#include "./ProcessorRhythm.h"

#include "./ProcessorRhythmKeys.h"
#include "./ProcessorRhythmValues.h"
#include "./ProcessorRhythmNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = RhythmPrKey::prefix;
    const juce::String prefixName = RhythmPrName::prefix;

    PrHelper::addRhythmLevelParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);

    // Create parameters for each of the 8 pads
    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        juce::String padPrefix = prefix + RhythmPrKey::pad + juce::String(i);
        juce::String padPrefixName = prefixName + RhythmPrName::pad + juce::String(i + 1);

        PrHelper::addRhythmPadVolParameters(layout, padPrefix, padPrefixName);
        PrHelper::addRhythmPadBasicParameters(layout, padPrefix, padPrefixName, CPV::Note::initial + i);
        PrHelper::addTnParameters(layout, padPrefix, padPrefixName);
        PrHelper::addAdpcmPanParameters(layout, padPrefix, padPrefixName);
        PrHelper::addPcmParameters(layout, padPrefix, padPrefixName);
        PrHelper::addLPParameters(layout, padPrefix, padPrefixName);
        PrHelper::addEnvBypassParameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgSwEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgSwEnv11Parameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgSwPEnv11Parameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpzx7LfoParameters(layout, padPrefix, padPrefixName);
        PrHelper::addFixParameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpzx7DetuneParameters(layout, padPrefix, padPrefixName);
        PrHelper::addQualityPcmParameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgHwEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addWtModParameters(layout, padPrefix, padPrefixName);
    }
}

void RhythmProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = RhythmPrKey::prefix;

    PrHelper::setupRhythmBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
    
    for (int i = 0; i < RhythmPrValue::pads; i++) {
        juce::String padPrefix = prefix + RhythmPrKey::pad + juce::String(i);

        PrHelper::setupRhythmPadBasicPtrs(apvts, padPrefix, pPadBasic[i]);
        PrHelper::setupQualityPcmPtrs(apvts, padPrefix, pQuality[i]);
        PrHelper::setupAdsrAmpEnvPtrs(apvts, padPrefix, pAmpEnv[i]);
        PrHelper::setupWtMod(apvts, padPrefix, pWtMod[i], modWaves);
        PrHelper::setupPitchEnvPtrs(apvts, padPrefix, pPitchEnv[i]);
        PrHelper::setupSsgSwEnvPtrs(apvts, padPrefix, pSsgSwEnv[i]);
        PrHelper::setupSsgSwEnv11Ptrs(apvts, padPrefix, pSsgSwEnv11[i]);
        PrHelper::setupSsgSwPEnv11Ptrs(apvts, padPrefix, pSsgSwPEnv11[i]);
        PrHelper::setupOpzx7DetunePtrs(apvts, padPrefix, pOpzx7Detune[i]);
        PrHelper::setupFixPtrs(apvts, padPrefix, pFix[i]);
        PrHelper::setupOpzx7LfoPtrs(apvts, padPrefix, pOpzx7Lfo[i]);
        PrHelper::setupPcm(apvts, padPrefix, pPcm[i]);
        PrHelper::setupLp(apvts, padPrefix, pLp[i]);
        PrHelper::setupToneNoise(apvts, padPrefix, pToneNoise[i]);
        PrHelper::setupSsgHwEnv(apvts, padPrefix, pSsgHwEnv[i]);
    }
}

void RhythmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyRhythmBasic(pBasic, params.rhythm);
    PrHelper::applyUnison(pUnison, params.rhythm.unison);

    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        auto& pad = params.rhythm.pads[i];

        PrHelper::applyRhythmPadBasic(pPadBasic[i], pad);
        PrHelper::applyQualityPcm(pQuality[i], pad.quality);
        PrHelper::applyAdsrAmpEnv(pAmpEnv[i], pad.adsr);
        PrHelper::applyWtMod(pWtMod[i], pad.wtMod);
        PrHelper::applySsgSwEnv(pSsgSwEnv[i], pad.ssgSwEnv);
        PrHelper::applySsgSwEnv11(pSsgSwEnv11[i], pad.ssgSwEnv11);
        PrHelper::applyPitchEnv(pPitchEnv[i], pad.pitchAdsr);
        PrHelper::applySsgSwPEnv11(pSsgSwPEnv11[i], pad.ssgSwPEnv11);
        PrHelper::applyOpzx7Detune(pOpzx7Detune[i], pad.detune);
        PrHelper::applyOpzx7Lfo(pOpzx7Lfo[i], pad.lfo);
        PrHelper::applyToneNoise(pToneNoise[i], pad.tn);
        PrHelper::applyPcm(pPcm[i], pad.pcm);
        PrHelper::applyLp(pLp[i], pad.lp);
        PrHelper::applyFix(pFix[i], pad.fix);
        PrHelper::applySsgHwEnv(pSsgHwEnv[i], pad.ssgHwEnv);
    }
}
