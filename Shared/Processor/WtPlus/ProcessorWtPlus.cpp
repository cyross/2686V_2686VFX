#include "./ProcessorWtPlus.h"

#include "./ProcessorWtPlusKeys.h"
#include "./ProcessorWtPlusNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void WtPlusProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = WtPlusPrKey::prefix;
    const juce::String prefixName = WtPlusPrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapParam::addParameters(layout, prefix, prefixName);
    ParamEnv::addParameters(layout, prefix, prefixName, ParamModCatalog::channelTargetCount(ParamModCatalog::wtPlus(), 0), 0);

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addWtPlusBasicParameters(layout, prefix, prefixName);
    // WT 系は階段のまま出していたので、INTERP の初期値は Zero-Order Hold。
    // これで 3.6.0 より前と同じ音になる。
    PrHelper::addQualityParameters(layout, prefix, prefixName, CPV::Quality::Interp::initialStepped);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
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
}

void WtPlusProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = WtPlusPrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);
    TapParam::setupPtrs(apvts, prefix, pTapParam, stepTables);
    ParamEnv::setupPtrs(apvts, prefix, pParamEnv, stepTables, ParamModCatalog::defaultsOf(apvts, ParamModCatalog::channelTargetIds(prefix, ParamModCatalog::wtPlus(), 0)));
    modResolver.init(apvts, prefix, ParamModCatalog::wtPlus(), 0);

    PrHelper::setupWtPlusBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    pAmpEnv.live = &stepTables.position(prefix + LiveKey::ampEnv);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    pPitchEnv.live = &stepTables.position(prefix + LiveKey::pitchEnv);
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

void WtPlusProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    // TAP PARAM SEQUENCER (オペレーターを持たないので、チャンネルの 1 つだけ)
    TapSeq::apply(pTapParam, params.tapParam);

    params.tapParamOpCount = 0;

    applyChannel(params.wtPlus);
}

// チャンネルの値。ボイスが PARAM ENV / TAP PARAM SEQUENCER で置き換えて読み直すときにも使う
void WtPlusProcessor::applyChannel(WtPlusParams& p)
{
    PrHelper::applyWtPlusBasic(pBasic, p);
    PrHelper::applyQuality(pQuality, p.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, p.adsr);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256, p.ssgSwEnv256);
    PrHelper::applyPitchEnv(pPitchEnv, p.pitchAdsr);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256, p.ssgSwPEnv256);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, p.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, p.lfo);
    PrHelper::applyFix(pFix, p.fix);
    PrHelper::applyWtMod(pMod, p.mod);
    PrHelper::applyWtAmpMod(pWtAmpMod, p.wtAmpMod);
    PrHelper::applySsgHwEnv(pSsgHwEnv, p.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, p.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, p.unison);

    // PARAM ENV[256] (チャンネル)
    modResolver.applyChannelEnv(pParamEnv, p.paramMod.envs[0]);
    p.paramMod.envCount = 1;
    p.paramMod.applier = this;
    modResolver.fillArp(p.paramMod);
}

// ボイスから。置き換えた値で、変わる区分だけ読み直す
void WtPlusProcessor::apply(void* chip, const ParamMod::Override* list, int count)
{
    auto& p = *static_cast<WtPlusParams*>(chip);

    std::array<PrOverride::Entry, ParamMod::maxOverrides> entries;
    ParamModResolver::Dirty dirty;

    const int n = modResolver.resolve(list, count, entries, dirty);

    if (n == 0) return;

    PrOverride::Scope scope(entries.data(), n);

    if (dirty.channel) applyChannel(p);
}
