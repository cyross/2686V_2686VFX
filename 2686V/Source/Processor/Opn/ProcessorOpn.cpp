#include "./ProcessorOpn.h"

#include "./ProcessorOpnKeys.h"
#include "./ProcessorOpnValues.h"
#include "./ProcessorOpnNames.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorHelper.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OpnPrKey::prefix;
    const juce::String prefixName = OpnPrName::prefix;

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
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PENV11。既定はバイパス。
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11BypassParameters(layout, prefix, prefixName);

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
        PrHelper::addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtAmpModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void OpnProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = OpnPrKey::prefix;

    PrHelper::setupOpnBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupN88LfoPtrs(apvts, prefix, pN88Lfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11g);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11g);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOpnDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpnAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupFixPtrs(apvts, p, pFix[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv11PtrsOp(apvts, p, pSsgSwEnv11[op]);
        PrHelper::setupSsgSwPEnv11PtrsOp(apvts, p, pSsgSwPEnv11[op]);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);

        pOpN88LfoAms[op] = PrHelper::setupOpN88AmsPtr(apvts, p);
        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
    }
}

void OpnProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpnBasic(pBasic, params.opn);
    PrHelper::applyQuality(pQuality, params.opn.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opn.algFb);
    PrHelper::applyN88Lfo(pN88Lfo, params.opn.glLfo);
    PrHelper::applyUnison(pUnison, params.opn.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opn.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opn.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opn.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opn.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.opn.wtAmpMod);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11g, params.opn.ssgSwEnv11g);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11g, params.opn.ssgSwPEnv11g);

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        PrHelper::applyOpnDetune(pOpDetune[op], params.opn.op[op].detune);
        PrHelper::applyOpnAdsr(pOpAdsr[op], params.opn.op[op].m_adsrParams);
        PrHelper::applyFix(pFix[op], params.opn.op[op].fix);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opn.op[op].ssgSwEnv, params.opn.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv11Op(pSsgSwEnv11[op], params.opn.op[op].ssgSwEnv11, params.opn.op[op].ssgEnv11Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opn.op[op].pitchAdsr, params.opn.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv11Op(pSsgSwPEnv11[op], params.opn.op[op].ssgSwPEnv11, params.opn.op[op].ssgPEnv11Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opn.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opn.op[op].wtAmpMod);

        params.opn.op[op].waveSelect = 0; // Sine
        params.opn.op[op].n88Lfo.ams = PrHelper::getInt(pOpN88LfoAms[op]);
        params.opn.op[op].mask = PrHelper::getBool(pOpMask[op]);
    }
}
