#include "./ProcessorWt2.h"

#include "./ProcessorWt2Keys.h"
#include "./ProcessorWt2Values.h"
#include "./ProcessorWt2Names.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void Wt2Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = Wt2PrKey::prefix;
    const juce::String prefixName = Wt2PrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapParam::addParameters(layout, prefix, prefixName);
    ParamEnv::addParameters(layout, prefix, prefixName, ParamModCatalog::channelTargetCount(ParamModCatalog::wt2(), 0), 0);

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addSpeedParameters(layout, prefix, prefixName);
    PrHelper::addWt2BasicParameters(layout, prefix, prefixName);
    // WT2 だけ INTERP の初期値が Linear で、WT・WT+ の Zero-Order Hold と違う。
    // 3.6.0 より前、WT2 の QUALITY は音源へ届いておらず (下の processBlock を
    // 参照)、線形補間で鳴っていたため。これまでと同じ音にするための初期値。
    PrHelper::addQualityParameters(layout, prefix, prefixName);
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
    PrHelper::addWt2CustomParameters(layout, prefix, prefixName);
}

void Wt2Processor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = Wt2PrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);
    TapParam::setupPtrs(apvts, prefix, pTapParam, stepTables);
    ParamEnv::setupPtrs(apvts, prefix, pParamEnv, stepTables);
    modResolver.init(apvts, prefix, ParamModCatalog::wt2(), 0);

    PrHelper::setupWt2BasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupWtCustom(apvts, prefix, pc);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnv);
    PrHelper::setupPitchEnvPtrs(apvts, prefix, pPitchEnv);
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

void Wt2Processor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    // TAP PARAM SEQUENCER (オペレーターを持たないので、チャンネルの 1 つだけ)
    TapSeq::apply(pTapParam, params.tapParam);

    params.tapParamOpCount = 0;

    applyChannel(params.wt2);
}

// チャンネルの値。ボイスが PARAM ENV / TAP PARAM SEQUENCER で置き換えて読み直すときにも使う
void Wt2Processor::applyChannel(Wt2Params& p)
{
    PrHelper::applyWt2Basic(pBasic, p);
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
    PrHelper::applyWt2CustomWave(pc, p);

    // PARAM ENV[256] (チャンネル)
    modResolver.applyChannelEnv(pParamEnv, p.paramMod.envs[0]);
    p.paramMod.envCount = 1;
    p.paramMod.applier = this;
    modResolver.fillArp(p.paramMod);
}

// ボイスから。置き換えた値で、変わる区分だけ読み直す
void Wt2Processor::apply(void* chip, const ParamMod::Override* list, int count)
{
    auto& p = *static_cast<Wt2Params*>(chip);

    std::array<PrOverride::Entry, ParamMod::maxOverrides> entries;
    ParamModResolver::Dirty dirty;

    const int n = modResolver.resolve(list, count, entries, dirty);

    if (n == 0) return;

    PrOverride::Scope scope(entries.data(), n);

    if (dirty.channel) applyChannel(p);
}
