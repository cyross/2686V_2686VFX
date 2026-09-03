#include "./ProcessorOpl3.h"

#include "./ProcessorOpl3Keys.h"
#include "./ProcessorOpl3Values.h"
#include "./ProcessorOpl3Names.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorHelper.h"

void Opl3Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = Opl3PrKey::prefix;
    const juce::String prefixName = Opl3PrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addAlgFbParameters(layout, prefix, prefixName, Opl3PrValue::Alg::max, Opl3PrValue::Alg::initial);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwEnvParameters(layout, prefix, prefixName);
    PrHelper::addSsgHwPEnvParameters(layout, prefix, prefixName);
    // チップ全体へ掛かる AMP ENV。既定はバイパス。
    PrHelper::addEnvParameters(layout, prefix, prefixName);
    PrHelper::addAdsrBypassParameter(layout, prefix, prefixName, true);
    // チップ全体へ掛かる MODULATION
    PrHelper::addWtModParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PENV11。既定はバイパス。
    PrHelper::addSsgSwPEnv11Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv11BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv11BypassParameters(layout, prefix, prefixName);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + CPK::op + juce::String(op);
        juce::String opPrefixName = prefixName + CPN::op + juce::String(op + 1);

        PrHelper::addOpOplDetuneParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOplLfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpl3EgParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOpl3AmpEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpEnvEnableParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void Opl3Processor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves) {
    const juce::String prefix = Opl3PrKey::prefix;

    PrHelper::setupOpl3BasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupSsgSwEnv11Ptrs(apvts, prefix, pSsgSwEnv11g);
    PrHelper::setupSsgSwPEnv11Ptrs(apvts, prefix, pSsgSwPEnv11g);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOplDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpl3AdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupOplEgPtrs(apvts, p, pOpEg[op]);
        PrHelper::setupOplLfoPtrs(apvts, p, pLfo[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv11PtrsOp(apvts, p, pSsgSwEnv11[op]);
        PrHelper::setupSsgSwPEnv11PtrsOp(apvts, p, pSsgSwPEnv11[op]);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);

        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
    }
}

void Opl3Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpl3Basic(pBasic, params.opl3);
    PrHelper::applyQuality(pQuality, params.opl3.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opl3.algFb);
    PrHelper::applyUnison(pUnison, params.opl3.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opl3.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opl3.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opl3.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opl3.wtMod);
    PrHelper::applySsgSwEnv11(pSsgSwEnv11g, params.opl3.ssgSwEnv11g);
    PrHelper::applySsgSwPEnv11(pSsgSwPEnv11g, params.opl3.ssgSwPEnv11g);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        PrHelper::applyOplDetune(pOpDetune[op], params.opl3.op[op].detune);
        PrHelper::applyOpl3Adsr(pOpAdsr[op], params.opl3.op[op].m_adsrParams);
        PrHelper::applyOplLfo(pLfo[op], params.opl3.op[op].lfo);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opl3.op[op].ssgSwEnv, params.opl3.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv11Op(pSsgSwEnv11[op], params.opl3.op[op].ssgSwEnv11, params.opl3.op[op].ssgEnv11Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opl3.op[op].pitchAdsr, params.opl3.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv11Op(pSsgSwPEnv11[op], params.opl3.op[op].ssgSwPEnv11, params.opl3.op[op].ssgPEnv11Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opl3.op[op].ssgHwPEnv);

        params.opl3.op[op].waveSelect = PrHelper::getInt(pOpEg[op].eg);
        params.opl3.op[op].mask = PrHelper::getBool(pOpMask[op]);
    }
}
