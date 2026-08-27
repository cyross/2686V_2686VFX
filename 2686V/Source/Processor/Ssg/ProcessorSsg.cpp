#include "./ProcessorSsg.h"

#include "./ProcessorSsgKeys.h"
#include "./ProcessorSsgValues.h"
#include "./ProcessorSsgNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = SsgPrKey::prefix;
    const juce::String prefixName = SsgPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addSsgBasicParameters(layout, prefix, prefixName);
    PrHelper::addSsgDutyParameters(layout, prefix, prefixName);
    PrHelper::addSsgTriParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addTnParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
}

void SsgProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = SsgPrKey::prefix;

    PrHelper::setupSsgBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupWtMod(apvts, prefix, pWtMod);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnvPtrs(apvts, prefix, pSsgSwEnv);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupToneNoise(apvts, prefix, pToneNoise);
    PrHelper::setupSsgDuty(apvts, prefix, pDuty);
    PrHelper::setupSsgTri(apvts, prefix, pTri);
    PrHelper::setupSsgHwEnv(apvts, prefix, pHwEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void SsgProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applySsgBasic(pBasic, params.ssg);
    PrHelper::applyFix(pFix, params.ssg.fix);
    PrHelper::applyQuality(pQuality, params.ssg.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.ssg.adsr);
    PrHelper::applyWtMod(pWtMod, params.ssg.wtMod);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.ssg.ssgSwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11, params.ssg.ssgSwEnv11);
    PrHelper::applyPitchEnv(pPitchEnv, params.ssg.pitchAdsr);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11, params.ssg.ssgSwPEnv11);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.ssg.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.ssg.lfo);
    PrHelper::applyToneNoise(pToneNoise, params.ssg.tn);
    PrHelper::applySsgDuty(pDuty, params.ssg.duty);
    PrHelper::applySsgTri(pTri, params.ssg.tri);
    PrHelper::applySsgHwEnv(pHwEnv, params.ssg.env);
    PrHelper::applyUnison(pUnison, params.ssg.unison);
}
