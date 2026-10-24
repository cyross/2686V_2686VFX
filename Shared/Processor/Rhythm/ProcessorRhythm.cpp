#include "./ProcessorRhythm.h"

#include "./ProcessorRhythmKeys.h"
#include "Shared/Processor/Rhythm/ProcessorRhythmValues.h"
#include "./ProcessorRhythmNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = RhythmPrKey::prefix;
    const juce::String prefixName = RhythmPrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapParam::addParameters(layout, prefix, prefixName);
    ParamEnv::addParameters(layout, prefix, prefixName, ParamModCatalog::channelTargetCount(ParamModCatalog::rhythm(), m_pads), 0);

    PrHelper::addRhythmLevelParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);

    // Create parameters for each of the 8 pads
    for (int i = 0; i < m_pads; ++i) {
        juce::String padPrefix = prefix + RhythmPrKey::pad + juce::String(i);
        juce::String padPrefixName = prefixName + RhythmPrName::pad + juce::String(i + 1);

        PrHelper::addRhythmPadVolParameters(layout, padPrefix, padPrefixName);
        PrHelper::addRhythmPadBasicParameters(layout, padPrefix, padPrefixName, CPV::Note::initial + i);
        PrHelper::addTnParameters(layout, padPrefix, padPrefixName);
        PrHelper::addAdpcmPanParameters(layout, padPrefix, padPrefixName);
        PrHelper::addPcmParameters(layout, padPrefix, padPrefixName);
        PrHelper::addLPParameters(layout, padPrefix, padPrefixName);
        PrHelper::addEnvBypassParameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgSwEnv256Parameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgSwPEnv256Parameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpzx7LfoParameters(layout, padPrefix, padPrefixName);
        PrHelper::addFixParameters(layout, padPrefix, padPrefixName);
        PrHelper::addOpzx7DetuneParameters(layout, padPrefix, padPrefixName);
        PrHelper::addQualityPcmParameters(layout, padPrefix, padPrefixName, m_pcmBitInitial);
        PrHelper::addSsgHwEnvParameters(layout, padPrefix, padPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, padPrefix, padPrefixName);

        // TAP PARAM SEQUENCER (パッド)。指し先はそのパッドのパラメータ
        TapParam::addParameters(layout, padPrefix, padPrefixName);
        PrHelper::addWtModParameters(layout, padPrefix, padPrefixName);
        PrHelper::addWtAmpModParameters(layout, padPrefix, padPrefixName);
    }
}

void RhythmProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = RhythmPrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);
    TapParam::setupPtrs(apvts, prefix, pTapParam, stepTables);
    ParamEnv::setupPtrs(apvts, prefix, pParamEnv, stepTables);
    modResolver.init(apvts, prefix, ParamModCatalog::rhythm(), m_pads);

    PrHelper::setupRhythmBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);
    
    for (int i = 0; i < m_pads; i++) {
        juce::String padPrefix = prefix + RhythmPrKey::pad + juce::String(i);

        TapParam::setupPtrs(apvts, padPrefix, pOpTapParam[(size_t)i], stepTables);

        PrHelper::setupRhythmPadBasicPtrs(apvts, padPrefix, pPadBasic[i]);
        PrHelper::setupQualityPcmPtrs(apvts, padPrefix, pQuality[i]);
        PrHelper::setupAdsrAmpEnvPtrs(apvts, padPrefix, pAmpEnv[i]);
        PrHelper::setupWtMod(apvts, padPrefix, pWtMod[i], modWaves);
        PrHelper::setupWtAmpMod(apvts, padPrefix, pWtAmpMod[i], modWaves);
        PrHelper::setupPitchEnvPtrs(apvts, padPrefix, pPitchEnv[i]);
        PrHelper::setupSsgSwEnv256Ptrs(apvts, padPrefix, pSsgSwEnv256[i], stepTables);
        PrHelper::setupSsgSwPEnv256Ptrs(apvts, padPrefix, pSsgSwPEnv256[i], stepTables);
        PrHelper::setupOpzx7DetunePtrs(apvts, padPrefix, pOpzx7Detune[i]);
        PrHelper::setupFixPtrs(apvts, padPrefix, pFix[i]);
        PrHelper::setupOpzx7LfoPtrs(apvts, padPrefix, pOpzx7Lfo[i]);
        PrHelper::setupPcm(apvts, padPrefix, pPcm[i]);
        PrHelper::setupLp(apvts, padPrefix, pLp[i]);
        PrHelper::setupToneNoise(apvts, padPrefix, pToneNoise[i]);
        PrHelper::setupSsgHwEnv(apvts, padPrefix, pSsgHwEnv[i]);
        PrHelper::setupSsgHwPEnv(apvts, padPrefix, pSsgHwPEnv[i]);
    }
}

void RhythmProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    // TAP PARAM SEQUENCER (チャンネルと、パッドごと)
    TapSeq::apply(pTapParam, params.tapParam);

    params.tapParamOpCount = std::min(m_pads, ParamMod::maxOps);

    for (int i = 0; i < params.tapParamOpCount; ++i) TapSeq::apply(pOpTapParam[(size_t)i], params.tapParamOps[(size_t)i]);

    applyChannel(params.rhythm);

    for (int i = 0; i < m_pads; ++i) applyOp(params.rhythm, i);
}

// チャンネルの値。ボイスが PARAM ENV / TAP PARAM SEQUENCER で置き換えて読み直すときにも使う
void RhythmProcessor::applyChannel(RhythmParams& p)
{
    PrHelper::applyRhythmBasic(pBasic, p);
    PrHelper::applyUnison(pUnison, p.unison);

    // PARAM ENV[256] (チャンネル)
    modResolver.applyChannelEnv(pParamEnv, p.paramMod.envs[0]);
    p.paramMod.envCount = 1;
    p.paramMod.applier = this;
    modResolver.fillArp(p.paramMod);
}

// パッド 1 つぶんの値
void RhythmProcessor::applyOp(RhythmParams& p, int i)
{
    auto& pad = p.pads[i];

    PrHelper::applyRhythmPadBasic(pPadBasic[i], pad);
    PrHelper::applyQualityPcm(pQuality[i], pad.quality);
    PrHelper::applyAdsrAmpEnv(pAmpEnv[i], pad.adsr);
    PrHelper::applyWtMod(pWtMod[i], pad.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod[i], pad.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256[i], pad.ssgSwEnv256);
    PrHelper::applyPitchEnv(pPitchEnv[i], pad.pitchAdsr);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256[i], pad.ssgSwPEnv256);
    PrHelper::applyOpzx7Detune(pOpzx7Detune[i], pad.detune);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo[i], pad.lfo);
    PrHelper::applyToneNoise(pToneNoise[i], pad.tn);
    PrHelper::applyPcm(pPcm[i], pad.pcm);
    PrHelper::applyLp(pLp[i], pad.lp);
    PrHelper::applyFix(pFix[i], pad.fix);
    PrHelper::applySsgHwEnv(pSsgHwEnv[i], pad.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv[i], pad.ssgHwPEnv);
}

// ボイスから。置き換えた値で、変わる区分だけ読み直す
void RhythmProcessor::apply(void* chip, const ParamMod::Override* list, int count)
{
    auto& p = *static_cast<RhythmParams*>(chip);

    std::array<PrOverride::Entry, ParamMod::maxOverrides> entries;
    ParamModResolver::Dirty dirty;

    const int n = modResolver.resolve(list, count, entries, dirty);

    if (n == 0) return;

    PrOverride::Scope scope(entries.data(), n);

    if (dirty.channel) applyChannel(p);

    for (int i = 0; i < m_pads && i < ParamMod::maxEnvs; ++i) {
        if (dirty.ops[(size_t)i]) applyOp(p, i);
    }
}
