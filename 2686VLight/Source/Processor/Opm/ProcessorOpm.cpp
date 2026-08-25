
#include "./ProcessorOpm.h"

#include "./ProcessorOpmKeys.h"
#include "./ProcessorOpmValues.h"
#include "./ProcessorOpmNames.h"
#include "../../Core/Processor/ProcessorKeys.h"
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
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11BypassParameters(layout, prefix, prefixName);

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
        PrHelper::addOpSsgSwEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void OpmProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = OpmPrKey::prefix;

    PrHelper::setupOpmBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupOpmLfoPtrs(apvts, prefix, pGlLfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11g);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOpmDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpmAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupFixPtrs(apvts, p, pFix[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv11PtrsOp(apvts, p, pSsgSwEnv11[op]);
        PrHelper::setupSsgSwPEnv11PtrsOp(apvts, p, pSsgSwPEnv11[op]);

        pOpLfoAmsEnable[op] = PrHelper::setupOpOpmAmsEnPtr(apvts, p);
        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
    }
}

void OpmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpmBasic(pBasic, params.opm);
    PrHelper::applyOpmLfo(pGlLfo, params.opm.glLfo);
    PrHelper::applyQuality(pQuality, params.opm.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opm.algFb);
    PrHelper::applyUnison(pUnison, params.opm.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opm.ssgHwEnv);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11g, params.opm.ssgSwEnv11g);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        params.opm.op[op].se.eg = 0;
        params.opm.op[op].se.freq = 0;

        PrHelper::applyOpmDetune(pOpDetune[op], params.opm.op[op].detune);
        PrHelper::applyOpmAdsr(pOpAdsr[op], params.opm.op[op].m_adsrParams);
        PrHelper::applyFix(pFix[op], params.opm.op[op].fix);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opm.op[op].ssgSwEnv, params.opm.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv11Op(pSsgSwEnv11[op], params.opm.op[op].ssgSwEnv11, params.opm.op[op].ssgEnv11Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opm.op[op].pitchAdsr, params.opm.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv11Op(pSsgSwPEnv11[op], params.opm.op[op].ssgSwPEnv11, params.opm.op[op].ssgPEnv11Enable);

        params.opm.op[op].lfo.amsEnable = PrHelper::getBool(pOpLfoAmsEnable[op]);
        params.opm.op[op].mask = PrHelper::getBool(pOpMask[op]);
    }
}
