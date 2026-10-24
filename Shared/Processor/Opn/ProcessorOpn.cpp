#include "./ProcessorOpn.h"

#include "./ProcessorOpnKeys.h"
#include "Shared/Processor/Opn/ProcessorOpnValues.h"
#include "./ProcessorOpnNames.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorHelper.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OpnPrKey::prefix;
    const juce::String prefixName = OpnPrName::prefix;

    // TAP AMP / PITCH SEQUENCER
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::amp());
    TapSeq::addParameters(layout, prefix, prefixName, TapSeq::pitch());

    // TAP PARAM SEQUENCER / PARAM ENV[256] (チャンネル) (4.3.0)
    TapParam::addParameters(layout, prefix, prefixName);
    ParamEnv::addParameters(layout, prefix, prefixName, ParamModCatalog::channelTargetCount(ParamModCatalog::opn(), OpnPrValue::ops), 0);

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addAlgFbParameters(layout, prefix, prefixName, OpnPrValue::Alg::max, OpnPrValue::Alg::initial);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addN88LfoParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    // チップ全体へ掛かる AMP ENV。既定はバイパス。
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addAdsrBypassParameter(layout, prefix, prefixName, true);
    // チップ全体へ掛かる MODULATION
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PITCH ENV[256]。既定はバイパス。
    PrHelper::addSsgSwPEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv256BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256BypassParameters(layout, prefix, prefixName);
    ValueEnv::addParameters(layout, prefix, prefixName, ValueEnv::alg(OpnPrValue::algorithms));

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + CPK::op + juce::String(op);
        juce::String opPrefixName = prefixName + CPN::op + juce::String(op + 1);

        PrHelper::addOpOpnDetuneParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpN88LfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpnAmpEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpFixParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpEnvEnableParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv256Parameters(layout, opPrefix, opPrefixName);
        // PARAM ENV[256] (オペレーター)。4.0.0 の TL ENV の代わり。TARGET の初期値は TL
        ParamEnv::addParameters(layout, opPrefix, opPrefixName, (int)ParamModCatalog::opn().op.size(), ParamModCatalog::opn().tlIndex);
        PrHelper::addOpSsgSwPEnv256Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtAmpModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addDelayParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void OpnProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = OpnPrKey::prefix;

    TapSeq::setupPtrs(apvts, prefix, TapSeq::amp(), pTapAmp, stepTables);
    TapSeq::setupPtrs(apvts, prefix, TapSeq::pitch(), pTapPitch, stepTables);
    TapParam::setupPtrs(apvts, prefix, pTapParam, stepTables);
    ParamEnv::setupPtrs(apvts, prefix, pParamEnv, stepTables);
    modResolver.init(apvts, prefix, ParamModCatalog::opn(), OpnPrValue::ops);

    PrHelper::setupOpnBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupN88LfoPtrs(apvts, prefix, pN88Lfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256g, stepTables);
    ValueEnv::setupPtrs(apvts, prefix, ValueEnv::alg(OpnPrValue::algorithms), pAlgEnv, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256g, stepTables);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOpnDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpnAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupFixPtrs(apvts, p, pFix[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnv256PtrsOp(apvts, p, pSsgSwEnv256[op], stepTables);
        ParamEnv::setupPtrs(apvts, p, pOpParamEnv[op], stepTables);
        PrHelper::setupSsgSwPEnv256PtrsOp(apvts, p, pSsgSwPEnv256[op], stepTables);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpN88LfoAms[op] = PrHelper::setupOpN88AmsPtr(apvts, p);
        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
        pOpDelay[op] = apvts.getRawParameterValue(p + CPK::delay);
    }
}

void OpnProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // TAP AMP / PITCH SEQUENCER。札を見て段を動かし、ボイスへ渡す
    TapSeq::apply(pTapAmp, params.tapAmp);
    TapSeq::apply(pTapPitch, params.tapPitch);

    // TAP PARAM SEQUENCER
    TapSeq::apply(pTapParam, params.tapParam);

    applyChannel(params.opn);

    for (int op = 0; op < OpnPrValue::ops; ++op) applyOp(params.opn, op);
}

// チャンネルの値。ボイスが PARAM ENV / TAP PARAM SEQUENCER で置き換えて読み直すときにも使う
void OpnProcessor::applyChannel(OpnParams& p)
{
    PrHelper::applyOpnBasic(pBasic, p);
    PrHelper::applyQuality(pQuality, p.quality);
    PrHelper::applyAlgFb(pAlgFb, p.algFb);
    PrHelper::applyN88Lfo(pN88Lfo, p.glLfo);
    PrHelper::applyUnison(pUnison, p.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, p.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, p.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, p.ampEnvG);
    PrHelper::applyWtMod(pWtMod, p.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, p.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256g, p.ssgSwEnv256g);
    ValueEnv::apply(pAlgEnv, p.algEnv, p.algEnvEnable);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256g, p.ssgSwPEnv256g);

    // PARAM ENV[256] (チャンネル)
    modResolver.applyChannelEnv(pParamEnv, p.paramMod.envs[0]);
    p.paramMod.envCount = 1 + OpnPrValue::ops;
    p.paramMod.applier = this;
    modResolver.fillArp(p.paramMod);
}

// オペレーター 1 つぶんの値
void OpnProcessor::applyOp(OpnParams& p, int op)
{
    PrHelper::applyOpnDetune(pOpDetune[op], p.op[op].detune);
    PrHelper::applyOpnAdsr(pOpAdsr[op], p.op[op].m_adsrParams);
    PrHelper::applyFix(pFix[op], p.op[op].fix);
    PrHelper::applySsgSwEnv256Op(pSsgSwEnv256[op], p.op[op].ssgSwEnv256, p.op[op].ssgEnv256Enable);
    // PARAM ENV[256] (オペレーター)。TL を指しているときはオペレーターの TL ENV の道へ渡す
    modResolver.applyOpEnv(pOpParamEnv[op], op, p.paramMod.envs[(size_t)op + 1], p.op[op].tlEnvEnable, p.op[op].tlEnv, p.op[op].tlEnvMax);
    PrHelper::applyPitchEnvOp(pPitchEnv[op], p.op[op].pitchAdsr, p.op[op].pitchEnvEnable);
    PrHelper::applySsgSwPEnv256Op(pSsgSwPEnv256[op], p.op[op].ssgSwPEnv256, p.op[op].ssgPEnv256Enable);
    PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], p.op[op].ssgHwPEnv);
    PrHelper::applyWtAmpMod(pOpWtAmpMod[op], p.op[op].wtAmpMod);
    PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], p.op[op].ssgHwEnv);
    PrHelper::applyWtMod(pOpWtMod[op], p.op[op].wtMod);

    p.op[op].waveSelect = 0; // Sine
    p.op[op].n88Lfo.ams = PrHelper::getInt(pOpN88LfoAms[op]);
    p.op[op].mask = PrHelper::getBool(pOpMask[op]);
    p.op[op].delay = PrHelper::getFloat(pOpDelay[op]);
}

// ボイスから。置き換えた値で、変わる区分だけ読み直す
void OpnProcessor::apply(void* chip, const ParamMod::Override* list, int count)
{
    auto& p = *static_cast<OpnParams*>(chip);

    std::array<PrOverride::Entry, ParamMod::maxOverrides> entries;
    ParamModResolver::Dirty dirty;

    const int n = modResolver.resolve(list, count, entries, dirty);

    if (n == 0) return;

    PrOverride::Scope scope(entries.data(), n);

    if (dirty.channel) applyChannel(p);

    for (int op = 0; op < OpnPrValue::ops && op < ParamMod::maxEnvs; ++op) {
        if (dirty.ops[(size_t)op]) applyOp(p, op);
    }
}
