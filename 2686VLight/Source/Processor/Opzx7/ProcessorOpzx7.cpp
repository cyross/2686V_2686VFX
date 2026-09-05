#include "./ProcessorOpzx7.h"

#include "./ProcessorOpzx7Keys.h"
#include "./ProcessorOpzx7Values.h"
#include "./ProcessorOpzx7Names.h"
#include "../../Core/Processor/ProcessorKeys.h"
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
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PENV11。既定はバイパス。
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addSsgSwEnv11BypassParameters(layout, prefix, prefixName);

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
        PrHelper::addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtAmpModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpzx7LfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void Opzx7Processor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
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
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11g);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11g);
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
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pOpSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv11PtrsOp(apvts, p, pOpSsgSwEnv11[op]);
        PrHelper::setupSsgSwPEnv11PtrsOp(apvts, p, pOpSsgSwPEnv11[op]);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
    }
}

void Opzx7Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
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
    PrHelper::applySsgSwEnv11(pSsgSwEnv11g, params.opzx7.ssgSwEnv11g);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11g, params.opzx7.ssgSwPEnv11g);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        PrHelper::applyOpzx7Detune(pOpzx7Detune[op], params.opzx7.op[op].detune);
        PrHelper::applyOpzx7Adsr(pOpAdsr[op], params.opzx7.op[op].m_adsrParams);
        PrHelper::applySsgEg(pOpSsgEg[op], params.opzx7.op[op].se);
        PrHelper::applyFix(pFix[op], params.opzx7.op[op].fix);
        PrHelper::applyOpzx7Lfo(pOpOpzx7Lfo[op], params.opzx7.op[op].lfo);
        PrHelper::applyPcm(pPcm[op], params.opzx7.op[op].pcm);
        PrHelper::applyLp(pLp[op], params.opzx7.op[op].lp);
        PrHelper::applySsgSwEnvOp(pOpSsgSwEnv[op], params.opzx7.op[op].ssgSwEnv, params.opzx7.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv11Op(pOpSsgSwEnv11[op], params.opzx7.op[op].ssgSwEnv11, params.opzx7.op[op].ssgEnv11Enable);
        PrHelper::applyPitchEnvOp(pOpPitchEnv[op], params.opzx7.op[op].pitchAdsr, params.opzx7.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv11Op(pOpSsgSwPEnv11[op], params.opzx7.op[op].ssgSwPEnv11, params.opzx7.op[op].ssgPEnv11Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opzx7.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opzx7.op[op].wtAmpMod);
        PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], params.opzx7.op[op].ssgHwEnv);
        PrHelper::applyWtMod(pOpWtMod[op], params.opzx7.op[op].wtMod);

        params.opzx7.op[op].waveSelect = PrHelper::getInt(pOpWs[op].waveSelect);
        params.opzx7.op[op].mask = PrHelper::getBool(pOpMask[op]);
    }
}
