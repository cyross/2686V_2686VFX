
#include "./ProcessorOpm.h"

#include "./ProcessorOpmKeys.h"
#include "./ProcessorOpmValues.h"
#include "./ProcessorOpmNames.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OpmPrKey::prefix;
    const juce::String prefixName = OpmPrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpmPrKey::level, prefixName + OpmPrName::level, OpmPrValue::Level::min, OpmPrValue::Level::max, OpmPrValue::Level::initial));

    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::alg, prefixName + OpmPrName::alg, OpmPrValue::Alg::min, OpmPrValue::Alg::max, OpmPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::fb, prefixName + OpmPrName::fb, OpmPrValue::Fb::min, OpmPrValue::Fb::max, OpmPrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::bit, prefixName + OpmPrName::bit, OpmPrValue::Bit::min, OpmPrValue::Bit::max, OpmPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rate, prefixName + OpmPrName::rate, OpmPrValue::Rate::min, OpmPrValue::Rate::max, OpmPrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::freq, prefixName + OpmPrName::Lfo::freq, OpmPrValue::Lfo::Freq::min, OpmPrValue::Lfo::Freq::max, OpmPrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpmPrKey::Lfo::amSmoothRatio, prefixName + OpmPrName::Lfo::amSmoothRatio, OpmPrValue::Lfo::AmSmRt::min, OpmPrValue::Lfo::AmSmRt::max, OpmPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::pgShape, prefixName + OpmPrName::Lfo::pgShape, OpmPrValue::Lfo::PgShape::min, OpmPrValue::Lfo::PgShape::max, OpmPrValue::Lfo::PgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::egShape, prefixName + OpmPrName::Lfo::egShape, OpmPrValue::Lfo::EgShape::min, OpmPrValue::Lfo::EgShape::max, OpmPrValue::Lfo::EgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpmPrKey::Lfo::am, prefixName + OpmPrName::Lfo::am, OpmPrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpmPrKey::Lfo::pm, prefixName + OpmPrName::Lfo::pm, OpmPrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::pms, prefixName + OpmPrName::Lfo::pms, OpmPrValue::Lfo::Pms::min, OpmPrValue::Lfo::Pms::max, OpmPrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::ams, prefixName + OpmPrName::Lfo::ams, OpmPrValue::Lfo::Ams::min, OpmPrValue::Lfo::Ams::max, OpmPrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::pmd, prefixName + OpmPrName::Lfo::pmd, OpmPrValue::Lfo::Pmd::min, OpmPrValue::Lfo::Pmd::max, OpmPrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::amd, prefixName + OpmPrName::Lfo::amd, OpmPrValue::Lfo::Amd::min, OpmPrValue::Lfo::Amd::max, OpmPrValue::Lfo::Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Lfo::syncDelay, prefixName + OpmPrName::Lfo::syncDelay, OpmPrValue::Lfo::SyncDelayOpm::min, OpmPrValue::Lfo::SyncDelayOpm::max, OpmPrValue::Lfo::SyncDelayOpm::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::pan, prefixName + OpmPrName::pan, OpmPrValue::Pan::min, OpmPrValue::Pan::max, OpmPrValue::Pan::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Unison::voices, prefixName + OpmPrName::Unison::voices, OpmPrValue::Unison::Voices::min, OpmPrValue::Unison::Voices::max, OpmPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::Unison::detune, prefixName + OpmPrName::Unison::detune, OpmPrValue::Unison::Detune::min, OpmPrValue::Unison::Detune::max, OpmPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpmPrKey::Unison::spread, prefixName + OpmPrName::Unison::spread, OpmPrValue::Unison::Spread::min, OpmPrValue::Unison::Spread::max, OpmPrValue::Unison::Spread::initial));

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + OpmPrKey::op + juce::String(op);
        juce::String opPrefixName = prefixName + OpmPrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::mul, opPrefixName + OpmPrName::mul, OpmPrValue::Op::Mul::min, OpmPrValue::Op::Mul::max, OpmPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::dt, opPrefixName + OpmPrName::dt1, OpmPrValue::Op::Dt1::min, OpmPrValue::Op::Dt1::max, OpmPrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::dt2, opPrefixName + OpmPrName::dt2, OpmPrValue::Op::Dt2::min, OpmPrValue::Op::Dt2::max, OpmPrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OpmPrKey::seFreq, opPrefixName + OpmPrName::seFreq, OpmPrValue::Op::SeFreq::min, OpmPrValue::Op::SeFreq::max, OpmPrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::ks, opPrefixName + OpmPrName::ks, OpmPrValue::Op::Ks::min, OpmPrValue::Op::Ks::max, OpmPrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::amsEn, opPrefixName + OpmPrName::amsEn, OpmPrValue::Op::Lfo::AmsEn::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::fix, opPrefixName + OpmPrName::fix, OpmPrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OpmPrKey::fixFreq, opPrefixName + OpmPrName::fixFreq, OpmPrValue::Op::FixFreq::min, OpmPrValue::Op::FixFreq::max, OpmPrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::mask, opPrefixName + OpmPrName::mask, OpmPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::PitchAdsr::enable, opPrefixName + OpmPrName::PitchAdsr::enable, OpmPrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::SsgSwEnv::enable, opPrefixName + OpmPrName::SsgSwEnv::enable, OpmPrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::rgAr, opPrefixName + OpmPrName::rgAr, OpmPrValue::Op::RgAdsr::Ar::min, OpmPrValue::Op::RgAdsr::Ar::max, OpmPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::rgD1r, opPrefixName + OpmPrName::rgD1r, OpmPrValue::Op::RgAdsr::D1r::min, OpmPrValue::Op::RgAdsr::D1r::max, OpmPrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::rgD1l, opPrefixName + OpmPrName::rgD1l, OpmPrValue::Op::RgAdsr::D1l::min, OpmPrValue::Op::RgAdsr::D1l::max, OpmPrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::rgD2r, opPrefixName + OpmPrName::rgD2r, OpmPrValue::Op::RgAdsr::D2r::min, OpmPrValue::Op::RgAdsr::D2r::max, OpmPrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::rgRr, opPrefixName + OpmPrName::rgRr, OpmPrValue::Op::RgAdsr::Rr::min, OpmPrValue::Op::RgAdsr::Rr::max, OpmPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpmPrKey::rgTl, opPrefixName + OpmPrName::rgTl, OpmPrValue::Op::RgAdsr::Tl::min, OpmPrValue::Op::RgAdsr::Tl::max, OpmPrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::xof, opPrefixName + OpmPrName::xof, OpmPrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::kor, opPrefixName + OpmPrName::kor, OpmPrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpmPrKey::ampBypass, opPrefixName + OpmPrName::ampBypass, OpmPrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
    }
}

void OpmProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = OpmPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + OpmPrKey::level);

    pAlg = apvts.getRawParameterValue(prefix + OpmPrKey::alg);
    pFb = apvts.getRawParameterValue(prefix + OpmPrKey::fb);
    pDepth = apvts.getRawParameterValue(prefix + OpmPrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + OpmPrKey::rate);

    pLfoFreq = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::freq);
    pLfoSyncDelay = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::syncDelay);
    pLfoPmWave = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::pgShape);
    pLfoAmWave = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::egShape);
    pLfoAmSmRt = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::amSmoothRatio);
    pLfoAmEnable = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::am);
    pLfoPms = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::pms);
    pLfoPmd = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::pmd);
    pLfoPmEnable = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::pm);
    pLfoAms = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::ams);
    pLfoAmd = apvts.getRawParameterValue(prefix + OpmPrKey::Lfo::amd);

    pPan = apvts.getRawParameterValue(prefix + OpmPrKey::pan);

    pUnisonVoices = apvts.getRawParameterValue(prefix + OpmPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + OpmPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + OpmPrKey::Unison::spread);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String p = prefix + OpmPrKey::op + juce::String(op);

        pOpMultiple[op] = apvts.getRawParameterValue(p + OpmPrKey::mul);
        pOpDetune[op] = apvts.getRawParameterValue(p + OpmPrKey::dt);
        pOpDetune2[op] = apvts.getRawParameterValue(p + OpmPrKey::dt2);

        pOpAdsrBypass[op] = apvts.getRawParameterValue(p + OpmPrKey::ampBypass);
        pOpAdsrRgAr[op] = apvts.getRawParameterValue(p + OpmPrKey::rgAr);
        pOpAdsrRgD1r[op] = apvts.getRawParameterValue(p + OpmPrKey::rgD1r);
        pOpAdsrRgD1l[op] = apvts.getRawParameterValue(p + OpmPrKey::rgD1l);
        pOpAdsrRgD2r[op] = apvts.getRawParameterValue(p + OpmPrKey::rgD2r);
        pOpAdsrRgRr[op] = apvts.getRawParameterValue(p + OpmPrKey::rgRr);
        pOpAdsrRgTl[op] = apvts.getRawParameterValue(p + OpmPrKey::rgTl);
        pOpAdsrKs[op] = apvts.getRawParameterValue(p + OpmPrKey::ks);
        pOpAdsrXof[op] = apvts.getRawParameterValue(p + OpmPrKey::xof);
        pOpAdsrKor[op] = apvts.getRawParameterValue(p + OpmPrKey::kor);

        pOpFixEnable[op] = apvts.getRawParameterValue(p + OpmPrKey::fix);
        pOpFixFreq[op] = apvts.getRawParameterValue(p + OpmPrKey::fixFreq);

        pOpLfoAmsEnable[op] = apvts.getRawParameterValue(p + OpmPrKey::amsEn);

        pOpPitchEnvEnable[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::enable);
        pOpPitchEnvAr[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::ar);
        pOpPitchEnvDr[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::dr);
        pOpPitchEnvRr[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::rr);
        pOpPitchEnvStl[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::stl);
        pOpPitchEnvAtl[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::atl);
        pOpPitchEnvSsl[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::ssl);
        pOpPitchEnvRll[op] = apvts.getRawParameterValue(p + OpmPrKey::PitchAdsr::rll);

        pOpSsgSwEnvEnable[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::enable);
        pOpSsgSwEnvSteps[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::steps);
        pOpSsgSwEnvLoop[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::loop);
        pOpSsgSwEnvLoopTo[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::loopTo);
        pOpSsgSwEnvLoopCount[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::loopCount);
        pOpSsgSwEnvStl[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::stl);
        pOpSsgSwEnvR1[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::r1);
        pOpSsgSwEnvL1[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::l1);
        pOpSsgSwEnvR2[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::r2);
        pOpSsgSwEnvL2[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::l2);
        pOpSsgSwEnvR3[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::r3);
        pOpSsgSwEnvL3[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::l3);
        pOpSsgSwEnvR4[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::r4);
        pOpSsgSwEnvL4[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::l4);
        pOpSsgSwEnvR5[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::r5);
        pOpSsgSwEnvL5[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::l5);
        pOpSsgSwEnvR6[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::r6);
        pOpSsgSwEnvL6[op] = apvts.getRawParameterValue(p + OpmPrKey::SsgSwEnv::l6);

        pOpMask[op] = apvts.getRawParameterValue(p + OpmPrKey::mask);
    }
}

void OpmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.opm.level = pLevel->load(std::memory_order_relaxed);

    params.opm.algorithm = (int)pAlg->load(std::memory_order_relaxed);

    params.opm.feedback = pFb->load(std::memory_order_relaxed);

    params.opm.fmBitDepth = (int)pDepth->load(std::memory_order_relaxed);
    params.opm.fmRateIndex = (int)pRate->load(std::memory_order_relaxed);

    params.opm.lfoFreq = pLfoFreq->load(std::memory_order_relaxed);
    params.opm.lfoWave = 0;
    params.opm.lfoSyncDelay = pLfoSyncDelay->load(std::memory_order_relaxed);
    params.opm.pmEnable = (pLfoPmEnable->load(std::memory_order_relaxed) > OpmPrValue::boolThread);
    params.opm.pgLfoWave = (int)pLfoPmWave->load(std::memory_order_relaxed);
    params.opm.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.opm.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.opm.amEnable = (pLfoAmEnable->load(std::memory_order_relaxed) > OpmPrValue::boolThread);
    params.opm.egLfoWave = (int)pLfoAmWave->load(std::memory_order_relaxed);
    params.opm.lfoAmSmRt = pLfoAmSmRt->load(std::memory_order_relaxed);
    params.opm.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.opm.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);

    params.opm.pan = pPan->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.opm.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.opm.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.opm.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        params.opm.op[op].multiple = (int)pOpMultiple[op]->load(std::memory_order_relaxed);
        params.opm.op[op].detune = (int)pOpDetune[op]->load(std::memory_order_relaxed);
        params.opm.op[op].detune2 = (int)pOpDetune2[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgEg = 0;

        params.opm.op[op].m_adsrParams.ar = (int)pOpAdsrRgAr[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.d1r = (int)pOpAdsrRgD1r[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.d1l = (int)pOpAdsrRgD1l[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.d2r = (int)pOpAdsrRgD2r[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.rr = (int)pOpAdsrRgRr[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.tl = (int)pOpAdsrRgTl[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.ks = (int)pOpAdsrKs[op]->load(std::memory_order_relaxed);
        params.opm.op[op].m_adsrParams.xof = (pOpAdsrXof[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread);
        params.opm.op[op].m_adsrParams.kor = pOpAdsrKor[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread;
        params.opm.op[op].m_adsrParams.bypass = pOpAdsrBypass[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread;

        params.opm.op[op].fixedMode = (pOpFixEnable[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread);
        params.opm.op[op].fixedFreq = pOpFixFreq[op]->load(std::memory_order_relaxed);

        params.opm.op[op].amsEnable = (pOpLfoAmsEnable[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread);

        params.opm.op[op].pitchEnvEnable = pOpPitchEnvEnable[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread;
        params.opm.op[op].pitchAdsr.bypass = false;
        params.opm.op[op].pitchAdsr.ar = pOpPitchEnvAr[op]->load(std::memory_order_relaxed);
        params.opm.op[op].pitchAdsr.dr = pOpPitchEnvDr[op]->load(std::memory_order_relaxed);
        params.opm.op[op].pitchAdsr.rr = pOpPitchEnvRr[op]->load(std::memory_order_relaxed);
        params.opm.op[op].pitchAdsr.stl = (int)pOpPitchEnvStl[op]->load(std::memory_order_relaxed);
        params.opm.op[op].pitchAdsr.atl = (int)pOpPitchEnvAtl[op]->load(std::memory_order_relaxed);
        params.opm.op[op].pitchAdsr.ssl = (int)pOpPitchEnvSsl[op]->load(std::memory_order_relaxed);
        params.opm.op[op].pitchAdsr.rll = (int)pOpPitchEnvRll[op]->load(std::memory_order_relaxed);

        params.opm.op[op].ssgEnvEnable = (pOpSsgSwEnvEnable[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread);
        params.opm.op[op].ssgSwEnv.bypass = false;
        params.opm.op[op].ssgSwEnv.steps = (int)pOpSsgSwEnvSteps[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.loop = (pOpSsgSwEnvLoop[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread);
        params.opm.op[op].ssgSwEnv.loopTo = (int)pOpSsgSwEnvLoopTo[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.loopCount = (int)pOpSsgSwEnvLoopCount[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.stl = pOpSsgSwEnvStl[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.r1 = pOpSsgSwEnvR1[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.l1 = pOpSsgSwEnvL1[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.r2 = pOpSsgSwEnvR2[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.l2 = pOpSsgSwEnvL2[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.r3 = pOpSsgSwEnvR3[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.l3 = pOpSsgSwEnvL3[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.r4 = pOpSsgSwEnvR4[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.l4 = pOpSsgSwEnvL4[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.r5 = pOpSsgSwEnvR5[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.l5 = pOpSsgSwEnvL5[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.r6 = pOpSsgSwEnvR6[op]->load(std::memory_order_relaxed);
        params.opm.op[op].ssgSwEnv.l6 = pOpSsgSwEnvL6[op]->load(std::memory_order_relaxed);

        params.opm.op[op].mask = pOpMask[op]->load(std::memory_order_relaxed) > OpmPrValue::boolThread;
    }
}
