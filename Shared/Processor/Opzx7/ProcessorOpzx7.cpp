#include "./ProcessorOpzx7.h"

#include "./ProcessorOpzx7Keys.h"
#include "Shared/Processor/Opzx7/ProcessorOpzx7Values.h"
#include "./ProcessorOpzx7Names.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorHelper.h"

void Opzx7Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = Opzx7PrKey::prefix;
    const juce::String prefixName = Opzx7PrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7AlgFbParameters(layout, prefix, prefixName, Opzx7PrValue::Alg::max, Opzx7PrValue::Alg::initial);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7PanpotParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addOpzx7LfoParameters(layout, prefix, prefixName);
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
    PrHelper::addSsgSwEnv256BypassParameters(layout, prefix, prefixName);
    ValueEnv::addParameters(layout, prefix, prefixName, ValueEnv::alg(Opzx7PrValue::algorithms));

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + CPK::op + juce::String(op);
        juce::String opPrefixName = prefixName + CPN::op + juce::String(op + 1);

        PrHelper::addOpzx7DetuneParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpnaSeParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpzx7WsParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPcmParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpLPParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpzx7AmpEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpFixParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpEnvEnableParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv256Parameters(layout, opPrefix, opPrefixName);
        ValueEnv::addParameters(layout, opPrefix, opPrefixName, ValueEnv::tl63());
        PrHelper::addOpSsgSwPEnv256Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtAmpModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpzx7LfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addDelayParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void Opzx7Processor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = Opzx7PrKey::prefix;

    PrHelper::setupOpzx7BasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupOpzx7AlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupPanpot(apvts, prefix, pPanpot);
    PrHelper::setupOpzx7LfoPtrs(apvts, prefix, pOpzx7Lfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256g, stepTables);
    ValueEnv::setupPtrs(apvts, prefix, ValueEnv::alg(Opzx7PrValue::algorithms), pAlgEnv, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256g, stepTables);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOpzx7DetunePtrs(apvts, p, pOpzx7Detune[op]);
        PrHelper::setupOpzx7AdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupSsgEgPtrs(apvts, p, pOpSsgEg[op]);
        PrHelper::setupWsPtrs(apvts, p, pOpWs[op]);
        PrHelper::setupFixPtrs(apvts, p, pFix[op]);
        PrHelper::setupOpzx7LfoPtrs(apvts, p, pOpOpzx7Lfo[op]);
        PrHelper::setupPcm(apvts, p, pPcm[op]);
        PrHelper::setupLp(apvts, p, pLp[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pOpPitchEnv[op]);
        PrHelper::setupSsgSwEnv256PtrsOp(apvts, p, pOpSsgSwEnv256[op], stepTables);
        ValueEnv::setupPtrs(apvts, p, ValueEnv::tl63(), pTlEnv[op], stepTables);
        PrHelper::setupSsgSwPEnv256PtrsOp(apvts, p, pOpSsgSwPEnv256[op], stepTables);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
        pOpDelay[op] = apvts.getRawParameterValue(p + CPK::delay);
    }
}

void Opzx7Processor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpzx7Basic(pBasic, params.opzx7);
    PrHelper::applyQuality(pQuality, params.opzx7.quality);
    PrHelper::applyOpzx7AlgFb(pAlgFb, params.opzx7.algFb);
    PrHelper::applyOpzx7Lfo(pOpzx7Lfo, params.opzx7.glLfo);
    PrHelper::applyPanpot(pPanpot, params.opzx7.panpot);
    PrHelper::applyUnison(pUnison, params.opzx7.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opzx7.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opzx7.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opzx7.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opzx7.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.opzx7.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256g, params.opzx7.ssgSwEnv256g);
    ValueEnv::apply(pAlgEnv, params.opzx7.algEnv, params.opzx7.algEnvEnable);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256g, params.opzx7.ssgSwPEnv256g);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        PrHelper::applyOpzx7Detune(pOpzx7Detune[op], params.opzx7.op[op].detune);
        PrHelper::applyOpzx7Adsr(pOpAdsr[op], params.opzx7.op[op].m_adsrParams);
        PrHelper::applySsgEg(pOpSsgEg[op], params.opzx7.op[op].se);
        PrHelper::applyFix(pFix[op], params.opzx7.op[op].fix);
        PrHelper::applyOpzx7Lfo(pOpOpzx7Lfo[op], params.opzx7.op[op].lfo);
        PrHelper::applyPcm(pPcm[op], params.opzx7.op[op].pcm);
        PrHelper::applyLp(pLp[op], params.opzx7.op[op].lp);
        PrHelper::applySsgSwEnv256Op(pOpSsgSwEnv256[op], params.opzx7.op[op].ssgSwEnv256, params.opzx7.op[op].ssgEnv256Enable);
        ValueEnv::apply(pTlEnv[op], params.opzx7.op[op].tlEnv, params.opzx7.op[op].tlEnvEnable);
        PrHelper::applyPitchEnvOp(pOpPitchEnv[op], params.opzx7.op[op].pitchAdsr, params.opzx7.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv256Op(pOpSsgSwPEnv256[op], params.opzx7.op[op].ssgSwPEnv256, params.opzx7.op[op].ssgPEnv256Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opzx7.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opzx7.op[op].wtAmpMod);
        PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], params.opzx7.op[op].ssgHwEnv);
        PrHelper::applyWtMod(pOpWtMod[op], params.opzx7.op[op].wtMod);

        params.opzx7.op[op].waveSelect = PrHelper::getInt(pOpWs[op].waveSelect);
        params.opzx7.op[op].mask = PrHelper::getBool(pOpMask[op]);
        params.opzx7.op[op].delay = PrHelper::getFloat(pOpDelay[op]);
    }
}
