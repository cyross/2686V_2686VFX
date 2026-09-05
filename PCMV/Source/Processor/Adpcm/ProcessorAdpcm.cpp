#include "./ProcessorAdpcm.h"

#include "./ProcessorAdpcmKeys.h"
#include "./ProcessorAdpcmValues.h"
#include "./ProcessorAdpcmNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = AdpcmPrKey::prefix;
    const juce::String prefixName = AdpcmPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addAdpcmBasicParameters(layout, prefix, prefixName);
    PrHelper::addTnParameters(layout, prefix, prefixName);
    PrHelper::addAdpcmPanParameters(layout, prefix, prefixName);
    PrHelper::addPcmParameters(layout, prefix, prefixName);
    PrHelper::addLPParameters(layout, prefix, prefixName);
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
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addQualityPcmParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
}

void AdpcmProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = AdpcmPrKey::prefix;

    PrHelper::setupAdpcmBasicPtrs(apvts, prefix, pBasic);
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
    PrHelper::setupToneNoise(apvts, prefix, pToneNoise);
    PrHelper::setupPcm(apvts, prefix, pPcm);
    PrHelper::setupLp(apvts, prefix, pLp);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyAdpcmBasic(pBasic, params.adpcm);
    PrHelper::applyQualityPcm(pQuality, params.adpcm.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.adpcm.adsr);
    PrHelper::applyWtMod(pWtMod, params.adpcm.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.adpcm.wtAmpMod);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.adpcm.ssgSwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11, params.adpcm.ssgSwEnv11);
    PrHelper::applyPitchEnv(pPitchEnv, params.adpcm.pitchAdsr);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11, params.adpcm.ssgSwPEnv11);
    PrHelper::applyFix(pFix, params.adpcm.fix);
    PrHelper::applyToneNoise(pToneNoise, params.adpcm.tn);
    PrHelper::applyPcm(pPcm, params.adpcm.pcm);
    PrHelper::applyLp(pLp, params.adpcm.lp);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.adpcm.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.adpcm.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, params.adpcm.unison);
}
