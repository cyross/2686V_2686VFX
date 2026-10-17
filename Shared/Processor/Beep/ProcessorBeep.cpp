#include "./ProcessorBeep.h"

#include "./ProcessorBeepKeys.h"
#include "./ProcessorBeepValues.h"
#include "./ProcessorBeepNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = BeepPrKey::prefix;
    const juce::String prefixName = BeepPrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addBool(
        layout,
        prefix + BeepPrKey::antiAlias,
        prefixName + BeepPrName::antiAlias,
        BeepPrValue::AntiAlias::initial
    );
    PrHelper::addInt(
        layout,
        prefix + BeepPrKey::timerClock,
        prefixName + BeepPrName::timerClock,
        BeepPrValue::TimerClock::min, BeepPrValue::TimerClock::max, BeepPrValue::TimerClock::initial
    );
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
}

void BeepProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = BeepPrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);

    PrHelper::setupBeepBasicPtrs(apvts, prefix, pBasic);
    pBasic.antiAlias = apvts.getRawParameterValue(prefix + BeepPrKey::antiAlias);
    pBasic.timerClock = apvts.getRawParameterValue(prefix + BeepPrKey::timerClock);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256, stepTables);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void BeepProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    PrHelper::applyBeepBasic(pBasic, params.beep);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, params.beep.adsr);
    PrHelper::applyWtMod(pWtMod, params.beep.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.beep.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256, params.beep.ssgSwEnv256);
    PrHelper::applyPitchEnv(pPitchEnv, params.beep.pitchAdsr);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256, params.beep.ssgSwPEnv256);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, params.beep.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.beep.lfo);
    PrHelper::applyFix(pFix, params.beep.fix);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.beep.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.beep.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, params.beep.unison);
}
