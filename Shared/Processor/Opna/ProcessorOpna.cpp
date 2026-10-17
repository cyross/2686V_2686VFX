#include "./ProcessorOpna.h"

#include "./ProcessorOpnaKeys.h"
#include "Shared/Processor/Opna/ProcessorOpnaValues.h"
#include "./ProcessorOpnaNames.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
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

void OpnaProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& modWaves, StepTableStore& stepTables) {
    const juce::String prefix = OpnaPrKey::prefix;

    PrHelper::setupOpnaBasicPtrs(apvts, prefix, pBasic);
    PrHelper::setupQualityPtrs(apvts, prefix, pQuality);
    PrHelper::setupAlgFbPtrs(apvts, prefix, pAlgFb);
    PrHelper::setupN88LfoPtrs(apvts, prefix, pN88Lfo);
    PrHelper::setupSsgHwEnv(apvts, prefix, pSsgHwEnv);
    PrHelper::setupSsgHwPEnv(apvts, prefix, pSsgHwPEnv);
    PrHelper::setupAdsrAmpEnvPtrs(apvts, prefix, pAmpEnvG);
    PrHelper::setupWtMod(apvts, prefix, pWtMod, modWaves);
    PrHelper::setupWtAmpMod(apvts, prefix, pWtAmpMod, modWaves);
    PrHelper::setupSsgSwEnv256Ptrs(apvts, prefix, pSsgSwEnv256g, stepTables);
    PrHelper::setupSsgSwPEnv256Ptrs(apvts, prefix, pSsgSwPEnv256g, stepTables);
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
        PrHelper::setupSsgSwEnv256PtrsOp(apvts, p, pSsgSwEnv256[op], stepTables);
        PrHelper::setupSsgSwPEnv256PtrsOp(apvts, p, pSsgSwPEnv256[op], stepTables);
        PrHelper::setupSsgHwPEnv(apvts, p, pOpSsgHwPEnv[op]);
        PrHelper::setupWtAmpMod(apvts, p, pOpWtAmpMod[op], modWaves);
        PrHelper::setupSsgHwEnv(apvts, p, pOpSsgHwEnv[op]);
        PrHelper::setupWtMod(apvts, p, pOpWtMod[op], modWaves);

        pOpN88LfoAms[op] = PrHelper::setupOpN88AmsPtr(apvts, p);
        pOpMask[op] = PrHelper::setupOpMaskPtr(apvts, p);
        pOpDelay[op] = apvts.getRawParameterValue(p + CPK::delay);
    }
}

void OpnaProcessor::processBlock(SynthCoreParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    PrHelper::applyOpnaBasic(pBasic, params.opna);
    PrHelper::applyQuality(pQuality, params.opna.quality);
    PrHelper::applyAlgFb(pAlgFb, params.opna.algFb);
    PrHelper::applyN88Lfo(pN88Lfo, params.opna.glLfo);
    PrHelper::applyUnison(pUnison, params.opna.unison);
    PrHelper::applySsgHwEnv(pSsgHwEnv, params.opna.ssgHwEnv);
    PrHelper::applySsgHwPEnv(pSsgHwPEnv, params.opna.ssgHwPEnv);
    PrHelper::applyAdsrAmpEnv(pAmpEnvG, params.opna.ampEnvG);
    PrHelper::applyWtMod(pWtMod, params.opna.wtMod);
    PrHelper::applyWtAmpMod(pWtAmpMod, params.opna.wtAmpMod);
    PrHelper::applySsgSwEnv256(pSsgSwEnv256g, params.opna.ssgSwEnv256g);
    PrHelper::applySsgSwPEnv256(pSsgSwPEnv256g, params.opna.ssgSwPEnv256g);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        PrHelper::applyOpnDetune(pOpDetune[op], params.opna.op[op].detune);
        PrHelper::applyOpnAdsr(pOpAdsr[op], params.opna.op[op].m_adsrParams);
        PrHelper::applySsgEg(pOpSsgEg[op], params.opna.op[op].se);
        PrHelper::applyFix(pFix[op], params.opna.op[op].fix);
        PrHelper::applyOpnaLfo(pLfo[op], params.opna.op[op].hwLfo);
        PrHelper::applySsgSwEnvOp(pSsgSwEnv[op], params.opna.op[op].ssgSwEnv, params.opna.op[op].ssgEnvEnable);
        PrHelper::applySsgSwEnv256Op(pSsgSwEnv256[op], params.opna.op[op].ssgSwEnv256, params.opna.op[op].ssgEnv256Enable);
        PrHelper::applyPitchEnvOp(pPitchEnv[op], params.opna.op[op].pitchAdsr, params.opna.op[op].pitchEnvEnable);
        PrHelper::applySsgSwPEnv256Op(pSsgSwPEnv256[op], params.opna.op[op].ssgSwPEnv256, params.opna.op[op].ssgPEnv256Enable);
        PrHelper::applySsgHwPEnv(pOpSsgHwPEnv[op], params.opna.op[op].ssgHwPEnv);
        PrHelper::applyWtAmpMod(pOpWtAmpMod[op], params.opna.op[op].wtAmpMod);
        PrHelper::applySsgHwEnv(pOpSsgHwEnv[op], params.opna.op[op].ssgHwEnv);
        PrHelper::applyWtMod(pOpWtMod[op], params.opna.op[op].wtMod);

        params.opna.op[op].waveSelect = 0; // Sine
        params.opna.op[op].n88Lfo.ams = PrHelper::getInt(pOpN88LfoAms[op]);
        params.opna.op[op].mask = PrHelper::getBool(pOpMask[op]);
        params.opna.op[op].delay = PrHelper::getFloat(pOpDelay[op]);
    }
}
