#include "./ProcessorAdpcmPlus.h"

#include "./ProcessorAdpcmPlusKeys.h"
#include "./ProcessorAdpcmPlusValues.h"
#include "./ProcessorAdpcmPlusNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void AdpcmPlusProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = AdpcmPlusPrKey::prefix;
    const juce::String prefixName = AdpcmPlusPrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapParam::addParameters(layout, prefix, prefixName);
    ParamEnv::addParameters(layout, prefix, prefixName, ParamModCatalog::channelTargetCount(ParamModCatalog::adpcmPlus(), 0), 0);

    PrHelper::addLevelParameters(layout, prefix, prefixName);

    // FORM・OPTIONAL・PAN は PCM ごと。まとめてここで並べる。
    PrHelper::addAdpcmPlusParameters(layout, prefix, prefixName);
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
    PrHelper::addQualityPcmParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
}

void AdpcmPlusProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = AdpcmPlusPrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);
    TapParam::setupPtrs(apvts, prefix, pTapParam, stepTables);
    ParamEnv::setupPtrs(apvts, prefix, pParamEnv, stepTables);
    modResolver.init(apvts, prefix, ParamModCatalog::adpcmPlus(), 0);

    PrHelper::setupAdpcmPlusPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPcmPtrs(apvts, prefix, pQuality);
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

void AdpcmPlusProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    // TAP PARAM SEQUENCER
    TapSeq::apply(pTapParam, params.tapParam);

    applyChannel(params.adpcmPlus);
}

// チャンネルの値。ボイスが PARAM ENV / TAP PARAM SEQUENCER で置き換えて読み直すときにも使う
void AdpcmPlusProcessor::applyChannel(AdpcmPlusParams& p)
{
    PrHelper::applyAdpcmPlusBasic(pBasic, p);
    PrHelper::applyQualityPcm(pQuality, p.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, p.adsr);
    PrHelper::applyWtMod(pWtMod, p.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, p.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256, p.ssgSwEnv256);
    PrHelper::applyPitchEnv(pPitchEnv, p.pitchAdsr);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256, p.ssgSwPEnv256);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, p.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, p.lfo);
    PrHelper::applyFix(pFix, p.fix);
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
void AdpcmPlusProcessor::apply(void* chip, const ParamMod::Override* list, int count)
{
    auto& p = *static_cast<AdpcmPlusParams*>(chip);

    std::array<PrOverride::Entry, ParamMod::maxOverrides> entries;
    ParamModResolver::Dirty dirty;

    const int n = modResolver.resolve(list, count, entries, dirty);

    if (n == 0) return;

    PrOverride::Scope scope(entries.data(), n);

    if (dirty.channel) applyChannel(p);
}
