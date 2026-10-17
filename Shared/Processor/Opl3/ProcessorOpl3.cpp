#include "./ProcessorOpl3.h"

#include "./ProcessorOpl3Keys.h"
#include "Shared/Processor/Opl3/ProcessorOpl3Values.h"
#include "./ProcessorOpl3Names.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
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
    PrHelper::addWtAmpModParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256Parameters(layout, prefix, prefixName);
    // チップ全体へ掛かる SSG SW PITCH ENV[256]。既定はバイパス。
    PrHelper::addSsgSwPEnv256Parameters(layout, prefix, prefixName);
    PrHelper::addSsgSwPEnv256BypassParameter(layout, prefix, prefixName, true);
    PrHelper::addUnisonParameters(layout, prefix, prefixName);
    PrHelper::addSsgSwEnv256BypassParameters(layout, prefix, prefixName);

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
        PrHelper::addOpSsgSwEnv256Parameters(layout, opPrefix, opPrefixName);
        ValueEnv::addParameters(layout, opPrefix, opPrefixName, ValueEnv::tl63());
        PrHelper::addOpSsgSwPEnv256Parameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwPEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtAmpModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addSsgHwEnvParameters(layout, opPrefix, opPrefixName);
        PrHelper::addWtModParameters(layout, opPrefix, opPrefixName);
        PrHelper::addDelayParameters(layout, opPrefix, opPrefixName);
        PrHelper::addOpMaskParameters(layout, opPrefix, opPrefixName);
    }
}

void Opl3Processor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = Opl3PrKey::prefix;

    PrHelper::setupOpl3BasicPtrs(apvts, prefix, pBasic);
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

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        juce::String p = prefix + CPK::op + juce::String(op);

        PrHelper::setupOplDetunePtrs(apvts, p, pOpDetune[op]);
        PrHelper::setupOpl3AdsrPtrs(apvts, p, pOpAdsr[op]);
        PrHelper::setupOplEgPtrs(apvts, p, pOpEg[op]);
        PrHelper::setupOplLfoPtrs(apvts, p, pLfo[op]);
        PrHelper::setupPitchEnvPtrsOp(apvts, p, pPitchEnv[op]);
        PrHelper::setupSsgSwEnv256PtrsOp(apvts, p, pSsgSwEnv256[op], stepTables);
        ValueEnv::setupPtrs(apvts, p, ValueEnv::tl63(), pTlEnv[op], stepTables);
        PrHelper::setupSsgSwPEnv256PtrsOp(apvts, p, pSsgSwPEnv256[op], stepTables);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
        pOpDelay[op] = apvts.getRawParameterValue(p + CPK::delay);
    }
}

void Opl3Processor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpl3Basic(pBasic, params.opl3);
    PrHelper::applyQuality(pQuality, params.opl3.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opl3.algFb);
    PrHelper::applyUnison(pUnison, params.opl3.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opl3.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opl3.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opl3.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opl3.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.opl3.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256g, params.opl3.ssgSwEnv256g);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256g, params.opl3.ssgSwPEnv256g);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        PrHelper::applyOplDetune(pOpDetune[op], params.opl3.op[op].detune);
        PrHelper::applyOpl3Adsr(pOpAdsr[op], params.opl3.op[op].m_adsrParams);
        PrHelper::applyOplLfo(pLfo[op], params.opl3.op[op].lfo);
        PrHelper::applySsgSwEnv256Op(pSsgSwEnv256[op], params.opl3.op[op].ssgSwEnv256, params.opl3.op[op].ssgEnv256Enable);
        ValueEnv::apply(pTlEnv[op], params.opl3.op[op].tlEnv, params.opl3.op[op].tlEnvEnable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opl3.op[op].pitchAdsr, params.opl3.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv256Op(pSsgSwPEnv256[op], params.opl3.op[op].ssgSwPEnv256, params.opl3.op[op].ssgPEnv256Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opl3.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opl3.op[op].wtAmpMod);
        PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], params.opl3.op[op].ssgHwEnv);
        PrHelper::applyWtMod(pOpWtMod[op], params.opl3.op[op].wtMod);

        params.opl3.op[op].waveSelect = PrHelper::getInt(pOpEg[op].eg);
        params.opl3.op[op].mask = PrHelper::getBool(pOpMask[op]);
        params.opl3.op[op].delay = PrHelper::getFloat(pOpDelay[op]);
    }
}
