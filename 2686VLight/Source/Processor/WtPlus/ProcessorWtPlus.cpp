#include "./ProcessorWtPlus.h"

#include "./ProcessorWtPlusKeys.h"
#include "./ProcessorWtPlusNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void WtPlusProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = WtPlusPrKey::prefix;
    const juce::String prefixName = WtPlusPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addWtPlusBasicParameters(layout, prefix, prefixName);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
}

void WtPlusProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = WtPlusPrKey::prefix;

    PrHelper::setupWtPlusBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnvPtrs(apvts, prefix, pSsgSwEnv);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupWtMod(apvts, prefix, pMod, modWaves);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void WtPlusProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyWtPlusBasic(pBasic, params.wtPlus);
    PrHelper::applyQuality(pQuality, params.wtPlus.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.wtPlus.adsr);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.wtPlus.ssgSwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11, params.wtPlus.ssgSwEnv11);
    PrHelper::applyPitchEnv(pPitchEnv, params.wtPlus.pitchAdsr);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11, params.wtPlus.ssgSwPEnv11);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.wtPlus.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.wtPlus.lfo);
    PrHelper::applyFix(pFix, params.wtPlus.fix);
    PrHelper::applyWtMod(pMod, params.wtPlus.mod);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.wtPlus.ssgHwEnv);
    PrHelper::applyUnison(pUnison, params.wtPlus.unison);
}
