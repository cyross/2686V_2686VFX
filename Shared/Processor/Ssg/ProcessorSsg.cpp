#include "./ProcessorSsg.h"

#include "./ProcessorSsgKeys.h"
#include "./ProcessorSsgValues.h"
#include "./ProcessorSsgNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = SsgPrKey::prefix;
    const juce::String prefixName = SsgPrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapParam::addParameters(layout, prefix, prefixName);
    ParamEnv::addParameters(layout, prefix, prefixName, ParamModCatalog::channelTargetCount(ParamModCatalog::ssg(), 0), 0);

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addSpeedParameters(layout, prefix, prefixName);
    PrHelper::addSsgBasicParameters(layout, prefix, prefixName);
    PrHelper::addSsgDutyParameters(layout, prefix, prefixName);
    PrHelper::addSsgTriParameters(layout, prefix, prefixName);
    ValueEnv::addParameters(layout, prefix, prefixName, ValueEnv::duty());
    ValueEnv::addParameters(layout, prefix, prefixName, ValueEnv::peak());
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addQualityParameters(layout, prefix, prefixName, CPV::Quality::Interp::initial, true);
    PrHelper::addTnParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addEnvBypassParameters(layout, prefix, prefixName);
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addPitchEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
    PrHelper::addFixParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7DetuneParameters(layout, prefix, prefixName);
}

void SsgProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = SsgPrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);
    TapParam::setupPtrs(apvts, prefix, pTapParam, stepTables);
    ParamEnv::setupPtrs(apvts, prefix, pParamEnv, stepTables);
    modResolver.init(apvts, prefix, ParamModCatalog::ssg(), 0);

    PrHelper::setupSsgBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256, stepTables);
    PrHelper::setupOpzx7DetunePtrs(apvts, prefix, pOpzx7Detune);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupFixPtrs(apvts, prefix, pFix);
    PrHelper::setupToneNoise(apvts, prefix, pToneNoise);
    PrHelper::setupSsgDuty(apvts, prefix, pDuty);
    PrHelper::setupSsgTri(apvts, prefix, pTri);
    ValueEnv::setupPtrs(apvts, prefix, ValueEnv::duty(), pDutyEnv, stepTables);
    ValueEnv::setupPtrs(apvts, prefix, ValueEnv::peak(), pPeakEnv, stepTables);
    PrHelper::setupSsgHwEnv(apvts, prefix, pHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pHwPEnv);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
}

void SsgProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    // TAP PARAM SEQUENCER (オペレーターを持たないので、チャンネルの 1 つだけ)
    TapSeq::apply(pTapParam, params.tapParam);

    params.tapParamOpCount = 0;

    applyChannel(params.ssg);
}

// チャンネルの値。ボイスが PARAM ENV / TAP PARAM SEQUENCER で置き換えて読み直すときにも使う
void SsgProcessor::applyChannel(SsgParams& p)
{
    PrHelper::applySsgBasic(pBasic, p);
    PrHelper::applyFix(pFix, p.fix);
    PrHelper::applyQuality(pQuality, p.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv, p.adsr);
    PrHelper::applyWtMod(pWtMod, p.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, p.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256, p.ssgSwEnv256);
    PrHelper::applyPitchEnv(pPitchEnv, p.pitchAdsr);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256, p.ssgSwPEnv256);
    PrHelper::applyOpzx7Detune(pOpzx7Detune, p.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, p.lfo);
    PrHelper::applyToneNoise(pToneNoise, p.tn);
    PrHelper::applySsgDuty(pDuty, p.duty);
    PrHelper::applySsgTri(pTri, p.tri);
    ValueEnv::apply(pDutyEnv, p.dutyEnv, p.dutyEnvEnable);
    ValueEnv::apply(pPeakEnv, p.peakEnv, p.peakEnvEnable);
    PrHelper::applySsgHwEnv(pHwEnv, p.env);
    PrHelper::applySsgHwPEnv(pHwPEnv, p.ssgHwPEnv);
    PrHelper::applyUnison(pUnison, p.unison);

    // PARAM ENV[256] (チャンネル)
    modResolver.applyChannelEnv(pParamEnv, p.paramMod.envs[0]);
    p.paramMod.envCount = 1;
    p.paramMod.applier = this;
    modResolver.fillArp(p.paramMod);
}

// ボイスから。置き換えた値で、変わる区分だけ読み直す
void SsgProcessor::apply(void* chip, const ParamMod::Override* list, int count)
{
    auto& p = *static_cast<SsgParams*>(chip);

    std::array<PrOverride::Entry, ParamMod::maxOverrides> entries;
    ParamModResolver::Dirty dirty;

    const int n = modResolver.resolve(list, count, entries, dirty);

    if (n == 0) return;

    PrOverride::Scope scope(entries.data(), n);

    if (dirty.channel) applyChannel(p);
}
