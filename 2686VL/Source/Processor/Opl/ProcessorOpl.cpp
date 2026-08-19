#include "./ProcessorOpl.h"

#include "./ProcessorOplKeys.h"
#include "./ProcessorOplValues.h"
#include "./ProcessorOplNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "../../Core/Processor/ProcessorValues.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OplPrKey::prefix;
    const juce::String prefixName = OplPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addAlgFbParameters(layout, prefix, prefixName, OplPrValue::Alg::max, OplPrValue::Alg::initial);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + CPK::op + juce::String(op);
        juce::String opPrefixName = prefixName + CPN::op + juce::String(op + 1);

        PrHelper::addOpOplDetuneParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOplLfoParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOplEgParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpOplAmpEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpEnvEnableParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpSsgSwPEnv11Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void OplProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = OplPrKey::prefix;

    PrHelper::setupOplBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOplDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOplAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupOplEgPtrs(apvts, p, pOpEg[op]);
        PrHelper::setupOplLfoPtrs(apvts, p, pLfo[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnvPtrsOp(apvts, p, pSsgSwEnv[op]);
        PrHelper::setupSsgSwEnv11PtrsOp(apvts, p, pSsgSwEnv11[op]);
        PrHelper::setupSsgSwPEnv11PtrsOp(apvts, p, pSsgSwPEnv11[op]);

        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOplBasic(pBasic, params.opl);
    PrHelper::applyQuality(pQuality, params.opl.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opl.algFb);
    PrHelper::applyUnison(pUnison, params.opl.unison);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        PrHelper::applyOplDetune(pOpDetune[op], params.opl.op[op].detune);
        PrHelper::applyOplAdsr(pOpAdsr[op], params.opl.op[op].m_adsrParams);
        PrHelper::applyOplLfo(pLfo[op], params.opl.op[op].lfo);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opl.op[op].ssgSwEnv, params.opl.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv11Op(pSsgSwEnv11[op], params.opl.op[op].ssgSwEnv11, params.opl.op[op].ssgEnv11Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opl.op[op].pitchAdsr, params.opl.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv11Op(pSsgSwPEnv11[op], params.opl.op[op].ssgSwPEnv11, params.opl.op[op].ssgPEnv11Enable);

        params.opl.op[op].waveSelect = PrHelper::getInt(pOpEg[op].eg);
        params.opl.op[op].mask = PrHelper::getBool(pOpMask[op]);
    }
}
