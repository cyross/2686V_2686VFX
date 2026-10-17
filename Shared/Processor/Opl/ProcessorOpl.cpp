#include "./ProcessorOpl.h"

#include "./ProcessorOplKeys.h"
#include "Shared/Processor/Opl/ProcessorOplValues.h"
#include "./ProcessorOplNames.h"
#include "../../Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorNames.h"
#include "Shared/Core/Processor/ProcessorValues.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OplPrKey::prefix;
    const juce::String prefixName = OplPrName::prefix;

    PrHelper::addLevelParameters(layout, prefix, prefixName);
    PrHelper::addAlgFbParameters(layout, prefix, prefixName, OplPrValue::Alg::max, OplPrValue::Alg::initial);
    PrHelper::addQualityParameters(layout, prefix, prefixName);
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

void OplProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = OplPrKey::prefix;

    PrHelper::setupOplBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256g, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256g, stepTables);
    PrHelper::setupUnisonPtrs(apvts, prefix, pUnison);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOplDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOplAdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupOplEgPtrs(apvts, p, pOpEg[op]);
        PrHelper::setupOplLfoPtrs(apvts, p, pLfo[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnv256PtrsOp(apvts, p, pSsgSwEnv256[op], stepTables);
        PrHelper::setupSsgSwPEnv256PtrsOp(apvts, p, pSsgSwPEnv256[op], stepTables);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
        pOpDelay[op] = apvts.getRawParameterValue(p + CPK::delay);
    }
}

void OplProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOplBasic(pBasic, params.opl);
    PrHelper::applyQuality(pQuality, params.opl.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opl.algFb);
    PrHelper::applyUnison(pUnison, params.opl.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opl.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opl.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opl.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opl.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.opl.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256g, params.opl.ssgSwEnv256g);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256g, params.opl.ssgSwPEnv256g);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        PrHelper::applyOplDetune(pOpDetune[op], params.opl.op[op].detune);
        PrHelper::applyOplAdsr(pOpAdsr[op], params.opl.op[op].m_adsrParams);
        PrHelper::applyOplLfo(pLfo[op], params.opl.op[op].lfo);
        PrHelper::applySsgSwEnv256Op(pSsgSwEnv256[op], params.opl.op[op].ssgSwEnv256, params.opl.op[op].ssgEnv256Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opl.op[op].pitchAdsr, params.opl.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv256Op(pSsgSwPEnv256[op], params.opl.op[op].ssgSwPEnv256, params.opl.op[op].ssgPEnv256Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opl.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opl.op[op].wtAmpMod);
        PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], params.opl.op[op].ssgHwEnv);
        PrHelper::applyWtMod(pOpWtMod[op], params.opl.op[op].wtMod);

        params.opl.op[op].waveSelect = PrHelper::getInt(pOpEg[op].eg);
        params.opl.op[op].mask = PrHelper::getBool(pOpMask[op]);
        params.opl.op[op].delay = PrHelper::getFloat(pOpDelay[op]);
    }
}
