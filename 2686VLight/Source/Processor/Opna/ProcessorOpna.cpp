#include "./ProcessorOpna.h"

#include "./ProcessorOpnaKeys.h"
#include "./ProcessorOpnaValues.h"
#include "./ProcessorOpnaNames.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorHelper.h"

void OpnaProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OpnaPrKey::prefix;
    const juce::String prefixName = OpnaPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addAlgFbParameters(layout, prefix, prefixName, OpnaPrValue::Alg::max, OpnaPrValue::Alg::initial);
    PrHelper::addOpnaPanParameters(layout, prefix, prefixName);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addN88LfoParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    // チップ全体へ掛かる AMP ENV。既定はバイパス。
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addAdsrBypassParameter(layout, prefix, prefixName, true);
    // チップ全体へ掛かる MODULATION
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PENV11。既定はバイパス。
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addSsgSwEnv11BypassParameters(layout, prefix, prefixName);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + CPK::op + juce::String(op);
        juce::String opPrefixName = prefixName + CPN::op + juce::String(op + 1);

        PrHelper::addOpOpnDetuneParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpnaSeParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpN88LfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpnaLfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpnAmpEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpFixParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpEnvEnableParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void OpnaProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = OpnaPrKey::prefix;

    PrHelper::setupOpnaBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupN88LfoPtrs(apvts, prefix, pN88Lfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11g);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11g);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOpnDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpnAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupSsgEgPtrs(apvts, p, pOpSsgEg[op]);
        PrHelper::setupFixPtrs(apvts, p, pFix[op]);
        PrHelper::setupOpnaLfoPtrs(apvts, p, pLfo[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv11PtrsOp(apvts, p, pSsgSwEnv11[op]);
        PrHelper::setupSsgSwPEnv11PtrsOp(apvts, p, pSsgSwPEnv11[op]);

        pOpN88LfoAms[op] = PrHelper::setupOpN88AmsPtr(apvts, p);
        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
    }
}

void OpnaProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpnaBasic(pBasic, params.opna);
    PrHelper::applyQuality(pQuality, params.opna.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opna.algFb);
    PrHelper::applyN88Lfo(pN88Lfo, params.opna.glLfo);
    PrHelper::applyUnison(pUnison, params.opna.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opna.ssgHwEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opna.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opna.wtMod);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11g, params.opna.ssgSwEnv11g);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11g, params.opna.ssgSwPEnv11g);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        PrHelper::applyOpnDetune(pOpDetune[op], params.opna.op[op].detune);
        PrHelper::applyOpnAdsr(pOpAdsr[op], params.opna.op[op].m_adsrParams);
        PrHelper::applySsgEg(pOpSsgEg[op], params.opna.op[op].se);
        PrHelper::applyFix(pFix[op], params.opna.op[op].fix);
        PrHelper::applyOpnaLfo(pLfo[op], params.opna.op[op].hwLfo);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opna.op[op].ssgSwEnv, params.opna.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv11Op(pSsgSwEnv11[op], params.opna.op[op].ssgSwEnv11, params.opna.op[op].ssgEnv11Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opna.op[op].pitchAdsr, params.opna.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv11Op(pSsgSwPEnv11[op], params.opna.op[op].ssgSwPEnv11, params.opna.op[op].ssgPEnv11Enable);

        params.opna.op[op].waveSelect = 0; // Sine
        params.opna.op[op].n88Lfo.ams = PrHelper::getInt(pOpN88LfoAms[op]);
        params.opna.op[op].mask = PrHelper::getBool(pOpMask[op]);
    }
}
