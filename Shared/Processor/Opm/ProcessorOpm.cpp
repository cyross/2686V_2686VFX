
#include "./ProcessorOpm.h"

#include "./ProcessorOpmKeys.h"
#include "Shared/Processor/Opm/ProcessorOpmValues.h"
#include "./ProcessorOpmNames.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorHelper.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OpmPrKey::prefix;
    const juce::String prefixName = OpmPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addOpmPanParameters(layout, prefix, prefixName);
    PrHelper::addAlgFbParameters(layout, prefix, prefixName, OpmPrValue::Alg::max, OpmPrValue::Alg::initial);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addOpmLfoParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    // チップ全体へ掛かる AMP ENV。既定はバイパス。
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addAdsrBypassParameter(layout, prefix, prefixName, true);
    // チップ全体へ掛かる MODULATION
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PENV11。既定はバイパス。
    PrHelper::addSsgSwPEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv256BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256BypassParameters(layout, prefix, prefixName);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + CPK::op + juce::String(op);
        juce::String opPrefixName = prefixName + CPN::op + juce::String(op + 1);

        PrHelper::addOpOpmDetuneParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpmSeParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpFixParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpmLfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpmAmpEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpEnvEnableParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv256Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv256Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtAmpModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addDelayParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void OpmProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = OpmPrKey::prefix;

    PrHelper::setupOpmBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupOpmLfoPtrs(apvts, prefix, pGlLfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256g);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256g);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOpmDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpmAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupFixPtrs(apvts, p, pFix[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv256PtrsOp(apvts, p, pSsgSwEnv256[op]);
        PrHelper::setupSsgSwPEnv256PtrsOp(apvts, p, pSsgSwPEnv256[op]);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpLfoAmsEnable[op] = PrHelper::setupOpOpmAmsEnPtr(apvts, p);
        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
        pOpDelay[op] = apvts.getRawParameterValue(p + CPK::delay);
    }
}

void OpmProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpmBasic(pBasic, params.opm);
    PrHelper::applyOpmLfo(pGlLfo, params.opm.glLfo);
    PrHelper::applyQuality(pQuality, params.opm.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opm.algFb);
    PrHelper::applyUnison(pUnison, params.opm.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opm.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opm.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opm.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opm.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.opm.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256g, params.opm.ssgSwEnv256g);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256g, params.opm.ssgSwPEnv256g);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        params.opm.op[op].se.eg = 0;
        params.opm.op[op].se.freq = 0;

        PrHelper::applyOpmDetune(pOpDetune[op], params.opm.op[op].detune);
        PrHelper::applyOpmAdsr(pOpAdsr[op], params.opm.op[op].m_adsrParams);
        PrHelper::applyFix(pFix[op], params.opm.op[op].fix);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opm.op[op].ssgSwEnv, params.opm.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv256Op(pSsgSwEnv256[op], params.opm.op[op].ssgSwEnv256, params.opm.op[op].ssgEnv256Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opm.op[op].pitchAdsr, params.opm.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv256Op(pSsgSwPEnv256[op], params.opm.op[op].ssgSwPEnv256, params.opm.op[op].ssgPEnv256Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opm.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opm.op[op].wtAmpMod);
        PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], params.opm.op[op].ssgHwEnv);
        PrHelper::applyWtMod(pOpWtMod[op], params.opm.op[op].wtMod);

        params.opm.op[op].lfo.amsEnable = PrHelper::getBool(pOpLfoAmsEnable[op]);
        params.opm.op[op].mask = PrHelper::getBool(pOpMask[op]);
        params.opm.op[op].delay = PrHelper::getFloat(pOpDelay[op]);
    }
}
