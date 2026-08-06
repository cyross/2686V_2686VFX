#include "./ProcessorBeep.h"

#include "./ProcessorBeepKeys.h"
#include "./ProcessorBeepValues.h"
#include "./ProcessorBeepNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = BeepPrKey::prefix;
    const juce::String prefixName = BeepPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
}

void BeepProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = BeepPrKey::prefix;

    PrHelper::setupBeepBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnvPtrs(apvts, prefix, pSsgSwEnv);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyBeepBasic(pBasic, params.beep);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.beep.adsr);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.beep.ssgSwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11, params.beep.ssgSwEnv11);
    PrHelper::applyPitchEnv(pPitchEnv, params.beep.pitchAdsr);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11, params.beep.ssgSwPEnv11);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.beep.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.beep.lfo);
    PrHelper::applyFix(pFix, params.beep.fix);
    PrHelper::applyUnison(pUnison, params.beep.unison);
}
