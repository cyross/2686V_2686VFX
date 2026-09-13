#include "./ProcessorAdpcmPlus.h"

#include "./ProcessorAdpcmPlusKeys.h"
#include "./ProcessorAdpcmPlusValues.h"
#include "./ProcessorAdpcmPlusNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void AdpcmPlusProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = AdpcmPlusPrKey::prefix;
    const juce::String prefixName = AdpcmPlusPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);

    // FORM・OPTIONAL・PAN は PCM ごと。まとめてここで並べる。
    PrHelper::addAdpcmPlusParameters(layout, prefix, prefixName);
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addQualityPcmParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
}

void AdpcmPlusProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = AdpcmPlusPrKey::prefix;

    PrHelper::setupAdpcmPlusPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPcmPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnvPtrs(apvts, prefix, pSsgSwEnv);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void AdpcmPlusProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyAdpcmPlusBasic(pBasic, params.adpcmPlus);
    PrHelper::applyQualityPcm(pQuality, params.adpcmPlus.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.adpcmPlus.adsr);
    PrHelper::applyWtMod(pWtMod, params.adpcmPlus.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.adpcmPlus.wtAmpMod);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.adpcmPlus.ssgSwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11, params.adpcmPlus.ssgSwEnv11);
    PrHelper::applyPitchEnv(pPitchEnv, params.adpcmPlus.pitchAdsr);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11, params.adpcmPlus.ssgSwPEnv11);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.adpcmPlus.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.adpcmPlus.lfo);
    PrHelper::applyFix(pFix, params.adpcmPlus.fix);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.adpcmPlus.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.adpcmPlus.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, params.adpcmPlus.unison);
}
