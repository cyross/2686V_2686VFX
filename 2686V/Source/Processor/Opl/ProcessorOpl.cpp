#include "./ProcessorOpl.h"

#include "./ProcessorOplKeys.h"
#include "./ProcessorOplValues.h"
#include "./ProcessorOplNames.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OplPrKey::prefix;
    const juce::String prefixName = OplPrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OplPrKey::level, prefixName + OplPrName::level, OplPrValue::Level::min, OplPrValue::Level::max, OplPrValue::Level::initial));

    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::alg, prefixName + OplPrName::alg, OplPrValue::Alg::min, OplPrValue::Alg::max, OplPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::fb, prefixName + OplPrName::fb, OplPrValue::Fb::min, OplPrValue::Fb::max, OplPrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::bit, prefixName + OplPrName::bit, OplPrValue::Bit::min, OplPrValue::Bit::max, OplPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rate, prefixName + OplPrName::rate, OplPrValue::Rate::min, OplPrValue::Rate::max, OplPrValue::Rate::initial)); // Default 6 (16kHz)

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::Unison::voices, prefixName + OplPrName::Unison::voices, OplPrValue::Unison::Voices::min, OplPrValue::Unison::Voices::max, OplPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::Unison::detune, prefixName + OplPrName::Unison::detune, OplPrValue::Unison::Detune::min, OplPrValue::Unison::Detune::max, OplPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OplPrKey::Unison::spread, prefixName + OplPrName::Unison::spread, OplPrValue::Unison::Spread::min, OplPrValue::Unison::Spread::max, OplPrValue::Unison::Spread::initial));

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + OplPrKey::op + juce::String(op);
        juce::String opPrefixName = prefixName + OplPrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::mul, opPrefixName + OplPrName::mul, OplPrValue::Op::Mul::min, OplPrValue::Op::Mul::max, OplPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::am, opPrefixName + OplPrName::am, OplPrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::vib, opPrefixName + OplPrName::vib, OplPrValue::Op::Lfo::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OplPrKey::ams, opPrefixName + OplPrName::ams, OplPrValue::Op::Lfo::Ams::min, OplPrValue::Op::Lfo::Ams::max, OplPrValue::Op::Lfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OplPrKey::amd, opPrefixName + OplPrName::amd, OplPrValue::Op::Lfo::Amd::min, OplPrValue::Op::Lfo::Amd::max, OplPrValue::Op::Lfo::Amd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OplPrKey::pms, opPrefixName + OplPrName::pms, OplPrValue::Op::Lfo::Pms::min, OplPrValue::Op::Lfo::Pms::max, OplPrValue::Op::Lfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OplPrKey::pmd, opPrefixName + OplPrName::pmd, OplPrValue::Op::Lfo::Pmd::min, OplPrValue::Op::Lfo::Pmd::max, OplPrValue::Op::Lfo::Pmd::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::egType, opPrefixName + OplPrName::egType, OplPrValue::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::ksr, opPrefixName + OplPrName::ksr, OplPrValue::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::ksl, opPrefixName + OplPrName::ksl, OplPrValue::Op::Ksl::min, OplPrValue::Op::Ksl::max, OplPrValue::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::eg, opPrefixName + OplPrName::eg, OplPrValue::Op::Eg::min, OplPrValue::Op::Eg::max, OplPrValue::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::mask, opPrefixName + OplPrName::mask, OplPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::PitchAdsr::enable, opPrefixName + OplPrName::PitchAdsr::enable, OplPrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::SsgSwEnv::enable, opPrefixName + OplPrName::SsgSwEnv::enable, OplPrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::rgAr, opPrefixName + OplPrName::rgAr, OplPrValue::Op::RgAdsr::Ar::min, OplPrValue::Op::RgAdsr::Ar::max, OplPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::rgDr, opPrefixName + OplPrName::rgDr, OplPrValue::Op::RgAdsr::Dr::min, OplPrValue::Op::RgAdsr::Dr::max, OplPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::rgSl, opPrefixName + OplPrName::rgSl, OplPrValue::Op::RgAdsr::Sl::min, OplPrValue::Op::RgAdsr::Sl::max, OplPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::rgRr, opPrefixName + OplPrName::rgRr, OplPrValue::Op::RgAdsr::Rr::min, OplPrValue::Op::RgAdsr::Rr::max, OplPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OplPrKey::rgTl, opPrefixName + OplPrName::rgTl, OplPrValue::Op::RgAdsr::Tl::min, OplPrValue::Op::RgAdsr::Tl::max, OplPrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::sus, opPrefixName + OplPrName::sus, OplPrValue::Op::Sus::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::xof, opPrefixName + OplPrName::xof, OplPrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::kor, opPrefixName + OplPrName::kor, OplPrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OplPrKey::ampBypass, opPrefixName + OplPrName::ampBypass, OplPrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
    }
}

void OplProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = OplPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + OplPrKey::level);

    pAlg = apvts.getRawParameterValue(prefix + OplPrKey::alg);
    pFb = apvts.getRawParameterValue(prefix + OplPrKey::fb);
    pDepth = apvts.getRawParameterValue(prefix + OplPrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + OplPrKey::rate);

    pUnisonVoices = apvts.getRawParameterValue(prefix + OplPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + OplPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + OplPrKey::Unison::spread);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String p = prefix + OplPrKey::op + juce::String(op);

        pOpMutiple[op] = apvts.getRawParameterValue(p + OplPrKey::mul);

        pOpAdsrBypass[op] = apvts.getRawParameterValue(p + OplPrKey::ampBypass);
        pOpAdsrAr[op] = apvts.getRawParameterValue(p + OplPrKey::rgAr);
        pOpAdsrDr[op] = apvts.getRawParameterValue(p + OplPrKey::rgDr);
        pOpAdsrSl[op] = apvts.getRawParameterValue(p + OplPrKey::rgSl);
        pOpAdsrRr[op] = apvts.getRawParameterValue(p + OplPrKey::rgRr);
        pOpAdsrTl[op] = apvts.getRawParameterValue(p + OplPrKey::rgTl);
        pOpAdsrKsr[op] = apvts.getRawParameterValue(p + OplPrKey::ksr);
        pOpAdsrKsl[op] = apvts.getRawParameterValue(p + OplPrKey::ksl);
        pOpAdsrSus[op] = apvts.getRawParameterValue(p + OplPrKey::sus);
        pOpAdsrXof[op] = apvts.getRawParameterValue(p + OplPrKey::xof);
        pOpAdsrKor[op] = apvts.getRawParameterValue(p + OplPrKey::kor);
        pOpAdsrEgType[op] = apvts.getRawParameterValue(p + OplPrKey::egType);

        pOpLfoEg[op] = apvts.getRawParameterValue(p + OplPrKey::eg);
        pOpLfoPmEnable[op] = apvts.getRawParameterValue(p + OplPrKey::vib);
        pOpLfoPms[op] = apvts.getRawParameterValue(p + OplPrKey::pms);
        pOpLfoPmd[op] = apvts.getRawParameterValue(p + OplPrKey::pmd);
        pOpLfoAmEnable[op] = apvts.getRawParameterValue(p + OplPrKey::am);
        pOpLfoAms[op] = apvts.getRawParameterValue(p + OplPrKey::ams);
        pOpLfoAmd[op] = apvts.getRawParameterValue(p + OplPrKey::amd);

        pOpPitchEnvEnable[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::enable);
        pOpPitchEnvAr[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::ar);
        pOpPitchEnvDr[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::dr);
        pOpPitchEnvRr[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::rr);
        pOpPitchEnvStl[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::stl);
        pOpPitchEnvAtl[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::atl);
        pOpPitchEnvSsl[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::ssl);
        pOpPitchEnvRll[op] = apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::rll);

        pOpSsgSwEnvEnable[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::enable);
        pOpSsgSwEnvSteps[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::steps);
        pOpSsgSwEnvLoop[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::loop);
        pOpSsgSwEnvLoopTo[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::loopTo);
        pOpSsgSwEnvLoopCount[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::loopCount);
        pOpSsgSwEnvStl[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::stl);
        pOpSsgSwEnvR1[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r1);
        pOpSsgSwEnvL1[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l1);
        pOpSsgSwEnvR2[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r2);
        pOpSsgSwEnvL2[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l2);
        pOpSsgSwEnvR3[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r3);
        pOpSsgSwEnvL3[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l3);
        pOpSsgSwEnvR4[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r4);
        pOpSsgSwEnvL4[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l4);
        pOpSsgSwEnvR5[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r5);
        pOpSsgSwEnvL5[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l5);
        pOpSsgSwEnvR6[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r6);
        pOpSsgSwEnvL6[op] = apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l6);

        pOpMask[op] = apvts.getRawParameterValue(p + OplPrKey::mask);
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.opl.level = pLevel->load(std::memory_order_relaxed);

    params.opl.algorithm = (int)pAlg->load(std::memory_order_relaxed);
    params.opl.feedback = pFb->load(std::memory_order_relaxed);
    params.opl.fmBitDepth = (int)pDepth->load(std::memory_order_relaxed);
    params.opl.fmRateIndex = (int)pRate->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.opl.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.opl.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.opl.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        params.opl.op[op].multiple = (int)pOpMutiple[op]->load(std::memory_order_relaxed);
        params.opl.op[op].waveSelect = (int)pOpLfoEg[op]->load(std::memory_order_relaxed);
        params.opl.op[op].vibEnable = pOpLfoPmEnable[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].amEnable = pOpLfoAmEnable[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].ams = pOpLfoAms[op]->load(std::memory_order_relaxed);
        params.opl.op[op].amd = pOpLfoAmd[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pms = pOpLfoPms[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pmd = pOpLfoPmd[op]->load(std::memory_order_relaxed);
        params.opl.op[op].mask = pOpMask[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;

        params.opl.op[op].m_adsrParams.ar = (int)pOpAdsrAr[op]->load(std::memory_order_relaxed);
        params.opl.op[op].m_adsrParams.dr = (int)pOpAdsrDr[op]->load(std::memory_order_relaxed);
        params.opl.op[op].m_adsrParams.sl = (int)pOpAdsrSl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].m_adsrParams.rr = (int)pOpAdsrRr[op]->load(std::memory_order_relaxed);
        params.opl.op[op].m_adsrParams.tl = (int)pOpAdsrTl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].m_adsrParams.egType = pOpAdsrEgType[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.ksr = pOpAdsrKsr[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.ksl = (int)pOpAdsrKsl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].m_adsrParams.sus = pOpAdsrSus[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.xof = pOpAdsrXof[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.kor = pOpAdsrKor[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.bypass = pOpAdsrBypass[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;

        params.opl.op[op].pitchEnvEnable = pOpPitchEnvEnable[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].pitchAdsr.bypass = false;
        params.opl.op[op].pitchAdsr.ar = pOpPitchEnvAr[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pitchAdsr.dr = pOpPitchEnvDr[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pitchAdsr.rr = pOpPitchEnvRr[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pitchAdsr.stl = (int)pOpPitchEnvStl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pitchAdsr.atl = (int)pOpPitchEnvAtl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pitchAdsr.ssl = (int)pOpPitchEnvSsl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].pitchAdsr.rll = (int)pOpPitchEnvRll[op]->load(std::memory_order_relaxed);

        params.opl.op[op].ssgEnvEnable = pOpSsgSwEnvEnable[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].ssgSwEnv.bypass = false;
        params.opl.op[op].ssgSwEnv.steps = (int)pOpSsgSwEnvSteps[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.loop = pOpSsgSwEnvLoop[op]->load(std::memory_order_relaxed) > OplPrValue::boolThread;
        params.opl.op[op].ssgSwEnv.loopTo = (int)pOpSsgSwEnvLoopTo[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.loopCount = (int)pOpSsgSwEnvLoopCount[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.stl = pOpSsgSwEnvStl[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.r1 = pOpSsgSwEnvR1[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.l1 = pOpSsgSwEnvL1[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.r2 = pOpSsgSwEnvR2[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.l2 = pOpSsgSwEnvL2[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.r3 = pOpSsgSwEnvR3[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.l3 = pOpSsgSwEnvL3[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.r4 = pOpSsgSwEnvR4[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.l4 = pOpSsgSwEnvL4[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.r5 = pOpSsgSwEnvR5[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.l5 = pOpSsgSwEnvL5[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.r6 = pOpSsgSwEnvR6[op]->load(std::memory_order_relaxed);
        params.opl.op[op].ssgSwEnv.l6 = pOpSsgSwEnvL6[op]->load(std::memory_order_relaxed);
    }
}
