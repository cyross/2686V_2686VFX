#include "./ProcessorWt.h"

#include "./ProcessorWtKeys.h"
#include "./ProcessorWtValues.h"
#include "./ProcessorWtNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void WtProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = WtPrKey::prefix;
    const juce::String prefixName = WtPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addSpeedParameters(layout, prefix, prefixName);
    PrHelper::addWtBasicParameters(layout, prefix, prefixName);
    // WT 系は階段のまま出していたので、INTERP の初期値は Zero-Order Hold。
    // これで 3.6.0 より前と同じ音になる。
    PrHelper::addQualityParameters(layout, prefix, prefixName, CPV::Quality::Interp::initialStepped);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    PrHelper::addWtCustomParameters(layout, prefix, prefixName);
}

void WtProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = WtPrKey::prefix;

    PrHelper::setupWtBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupWtCustom(apvts, prefix, pc);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnvPtrs(apvts, prefix, pSsgSwEnv);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256, stepTables);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupWtMod(apvts, prefix, pMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void WtProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyWtBasic(pBasic, params.wt);
    PrHelper::applyQuality(pQuality, params.wt.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.wt.adsr);
    PrHelper::applySsgSwEnv(pSsgSwEnv, params.wt.ssgSwEnv);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256, params.wt.ssgSwEnv256);
    PrHelper::applyPitchEnv(pPitchEnv, params.wt.pitchAdsr);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256, params.wt.ssgSwPEnv256);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.wt.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.wt.lfo);
    PrHelper::applyFix(pFix, params.wt.fix);
    PrHelper::applyWtMod(pMod, params.wt.mod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.wt.wtAmpMod);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.wt.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.wt.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, params.wt.unison);
    PrHelper::applyWtCustomWave(pc, params.wt);
}
