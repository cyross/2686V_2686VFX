#include "./ProcessorWt2.h"

#include "./ProcessorWt2Keys.h"
#include "./ProcessorWt2Values.h"
#include "./ProcessorWt2Names.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void Wt2Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = Wt2PrKey::prefix;
    const juce::String prefixName = Wt2PrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addWt2BasicParameters(layout, prefix, prefixName);
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
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    PrHelper::addWt2CustomParameters(layout, prefix, prefixName);
}

void Wt2Processor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = Wt2PrKey::prefix;

    PrHelper::setupWt2BasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupWtCustom(apvts, prefix, pc);
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
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void Wt2Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyWt2Basic(pBasic, params.wt2);
    PrHelper::applyQuality(pQuality, params.wt.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.wt2.adsr);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.wt2.ssgSwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11, params.wt2.ssgSwEnv11);
    PrHelper::applyPitchEnv(pPitchEnv, params.wt2.pitchAdsr);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11, params.wt2.ssgSwPEnv11);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.wt2.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.wt2.lfo);
    PrHelper::applyFix(pFix, params.wt2.fix);
    PrHelper::applyWtMod(pMod, params.wt2.mod);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.wt2.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.wt2.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, params.wt2.unison);
    PrHelper::applyWt2CustomWave(pc, params.wt2);
}
