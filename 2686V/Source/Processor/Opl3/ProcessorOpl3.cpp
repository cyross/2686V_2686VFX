#include "./ProcessorOpl3.h"

#include "./ProcessorOpl3Keys.h"
#include "./ProcessorOpl3Values.h"
#include "./ProcessorOpl3Names.h"

void Opl3Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = Opl3PrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opl3PrKey::level, code + Opl3PrName::level, Opl3PrValue::Level::min, Opl3PrValue::Level::max, Opl3PrValue::Level::initial));

    // ==============================================================================
    // OPL3 Parameters (4 Ops, 8 Waveforms)
    // ==============================================================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::alg, code + Opl3PrName::alg, Opl3PrValue::Alg::min, Opl3PrValue::Alg::max, Opl3PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::fb, code + Opl3PrName::fb, Opl3PrValue::Fb::min, Opl3PrValue::Fb::max, Opl3PrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::bit, code + Opl3PrName::bit, Opl3PrValue::Bit::min, Opl3PrValue::Bit::max, Opl3PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::rate, code + Opl3PrName::rate, Opl3PrValue::Rate::min, Opl3PrValue::Rate::max, Opl3PrValue::Rate::initial)); // Default 6 (16kHz)

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::Unison::voices, code + Opl3PrName::Unison::voices, Opl3PrValue::Unison::Voices::min, Opl3PrValue::Unison::Voices::max, Opl3PrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::Unison::detune, code + Opl3PrName::Unison::detune, Opl3PrValue::Unison::Detune::min, Opl3PrValue::Unison::Detune::max, Opl3PrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opl3PrKey::Unison::spread, code + Opl3PrName::Unison::spread, Opl3PrValue::Unison::Spread::min, Opl3PrValue::Unison::Spread::max, Opl3PrValue::Unison::Spread::initial));

    for (int i = 0; i < Opl3PrValue::ops; ++i)
    {
        juce::String prefix = code + Opl3PrKey::op + juce::String(i);
        juce::String namePrefix = code + Opl3PrName::op + juce::String(i + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::mul, namePrefix + Opl3PrName::mul, Opl3PrValue::Op::Mul::min, Opl3PrValue::Op::Mul::max, Opl3PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::am, namePrefix + Opl3PrName::am, Opl3PrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::vib, namePrefix + Opl3PrName::vib, Opl3PrValue::Op::Lfo::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opl3PrKey::ams, namePrefix + Opl3PrName::ams, Opl3PrValue::Op::Lfo::Ams::min, Opl3PrValue::Op::Lfo::Ams::max, Opl3PrValue::Op::Lfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opl3PrKey::amd, namePrefix + Opl3PrName::amd, Opl3PrValue::Op::Lfo::Amd::min, Opl3PrValue::Op::Lfo::Amd::max, Opl3PrValue::Op::Lfo::Amd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opl3PrKey::pms, namePrefix + Opl3PrName::pms, Opl3PrValue::Op::Lfo::Pms::min, Opl3PrValue::Op::Lfo::Pms::max, Opl3PrValue::Op::Lfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opl3PrKey::pmd, namePrefix + Opl3PrName::pmd, Opl3PrValue::Op::Lfo::Pmd::min, Opl3PrValue::Op::Lfo::Pmd::max, Opl3PrValue::Op::Lfo::Pmd::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::egType, namePrefix + Opl3PrName::egType, Opl3PrValue::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::ksr, namePrefix + Opl3PrName::ksr, Opl3PrValue::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::ksl, namePrefix + Opl3PrName::ksl, Opl3PrValue::Op::Ksl::min, Opl3PrValue::Op::Ksl::max, Opl3PrValue::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::eg, namePrefix + Opl3PrName::eg, Opl3PrValue::Op::Eg::min, Opl3PrValue::Op::Eg::max, Opl3PrValue::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::mask, namePrefix + Opl3PrName::mask, Opl3PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::PitchAdsr::enable, namePrefix + Opl3PrName::PitchAdsr::enable, Opl3PrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::SsgSwEnv::enable, namePrefix + Opl3PrName::SsgSwEnv::enable, Opl3PrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgAr, namePrefix + Opl3PrName::rgAr, Opl3PrValue::Op::RgAdsr::Ar::min, Opl3PrValue::Op::RgAdsr::Ar::max, Opl3PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgDr, namePrefix + Opl3PrName::rgDr, Opl3PrValue::Op::RgAdsr::Dr::min, Opl3PrValue::Op::RgAdsr::Dr::max, Opl3PrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgSl, namePrefix + Opl3PrName::rgSl, Opl3PrValue::Op::RgAdsr::Sl::min, Opl3PrValue::Op::RgAdsr::Sl::max, Opl3PrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgRr, namePrefix + Opl3PrName::rgRr, Opl3PrValue::Op::RgAdsr::Rr::min, Opl3PrValue::Op::RgAdsr::Rr::max, Opl3PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgTl, namePrefix + Opl3PrName::rgTl, Opl3PrValue::Op::RgAdsr::Tl::min, Opl3PrValue::Op::RgAdsr::Tl::max, Opl3PrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::xof, namePrefix + Opl3PrName::xof, Opl3PrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::kor, namePrefix + Opl3PrName::kor, Opl3PrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opl3PrKey::ampBypass, namePrefix + Opl3PrName::ampBypass, Opl3PrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
        addOpSsgSwEnvParameters(layout, prefix, namePrefix);
    }
}

void Opl3Processor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = Opl3PrKey::prefix;

    pLevel = apvts.getRawParameterValue(code + Opl3PrKey::level);

    pAlg = apvts.getRawParameterValue(code + Opl3PrKey::alg);
    pFb = apvts.getRawParameterValue(code + Opl3PrKey::fb);
    pDepth = apvts.getRawParameterValue(code + Opl3PrKey::bit);
    pRate = apvts.getRawParameterValue(code + Opl3PrKey::rate);

    pUnisonVoices = apvts.getRawParameterValue(code + Opl3PrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(code + Opl3PrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(code + Opl3PrKey::Unison::spread);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        juce::String p = code + Opl3PrKey::op + juce::String(op);

        pOpMutiple[op] = apvts.getRawParameterValue(p + Opl3PrKey::mul);

        pOpAdsrBypass[op] = apvts.getRawParameterValue(p + Opl3PrKey::ampBypass);
        pOpAdsrAr[op] = apvts.getRawParameterValue(p + Opl3PrKey::rgAr);
        pOpAdsrDr[op] = apvts.getRawParameterValue(p + Opl3PrKey::rgDr);
        pOpAdsrSl[op] = apvts.getRawParameterValue(p + Opl3PrKey::rgSl);
        pOpAdsrRr[op] = apvts.getRawParameterValue(p + Opl3PrKey::rgRr);
        pOpAdsrTl[op] = apvts.getRawParameterValue(p + Opl3PrKey::rgTl);
        pOpAdsrKsr[op] = apvts.getRawParameterValue(p + Opl3PrKey::ksr);
        pOpAdsrKsl[op] = apvts.getRawParameterValue(p + Opl3PrKey::ksl);
        pOpAdsrXof[op] = apvts.getRawParameterValue(p + Opl3PrKey::xof);
        pOpAdsrKor[op] = apvts.getRawParameterValue(p + Opl3PrKey::kor);
        pOpAdsrEgType[op] = apvts.getRawParameterValue(p + Opl3PrKey::egType);

        pOpLfoEg[op] = apvts.getRawParameterValue(p + Opl3PrKey::eg);
        pOpLfoPmEnable[op] = apvts.getRawParameterValue(p + Opl3PrKey::vib);
        pOpLfoPms[op] = apvts.getRawParameterValue(p + Opl3PrKey::pms);
        pOpLfoPmd[op] = apvts.getRawParameterValue(p + Opl3PrKey::pmd);
        pOpLfoAmEnable[op] = apvts.getRawParameterValue(p + Opl3PrKey::am);
        pOpLfoAms[op] = apvts.getRawParameterValue(p + Opl3PrKey::ams);
        pOpLfoAmd[op] = apvts.getRawParameterValue(p + Opl3PrKey::amd);

        pOpPitchEnvEnable[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::enable);
        pOpPitchEnvAr[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::ar);
        pOpPitchEnvDr[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::dr);
        pOpPitchEnvRr[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::rr);
        pOpPitchEnvStl[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::stl);
        pOpPitchEnvAtl[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::atl);
        pOpPitchEnvSsl[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::ssl);
        pOpPitchEnvRll[op] = apvts.getRawParameterValue(p + Opl3PrKey::PitchAdsr::rll);

        pOpSsgSwEnvEnable[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::enable);
        pOpSsgSwEnvSteps[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::steps);
        pOpSsgSwEnvLoop[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::loop);
        pOpSsgSwEnvLoopTo[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::loopTo);
        pOpSsgSwEnvLoopCount[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::loopCount);
        pOpSsgSwEnvStl[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::stl);
        pOpSsgSwEnvR1[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::r1);
        pOpSsgSwEnvL1[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::l1);
        pOpSsgSwEnvR2[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::r2);
        pOpSsgSwEnvL2[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::l2);
        pOpSsgSwEnvR3[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::r3);
        pOpSsgSwEnvL3[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::l3);
        pOpSsgSwEnvR4[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::r4);
        pOpSsgSwEnvL4[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::l4);
        pOpSsgSwEnvR5[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::r5);
        pOpSsgSwEnvL5[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::l5);
        pOpSsgSwEnvR6[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::r6);
        pOpSsgSwEnvL6[op] = apvts.getRawParameterValue(p + Opl3PrKey::SsgSwEnv::l6);

        pOpMask[op] = apvts.getRawParameterValue(p + Opl3PrKey::mask);
    }
}

void Opl3Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.opl3.level = pLevel->load(std::memory_order_relaxed);

    params.opl3.algorithm = (int)pAlg->load(std::memory_order_relaxed);
    params.opl3.feedback = pFb->load(std::memory_order_relaxed);
    params.opl3.fmBitDepth = (int)pDepth->load(std::memory_order_relaxed);
    params.opl3.fmRateIndex = (int)pRate->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.opl3.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.opl3.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.opl3.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        params.opl3.op[op].multiple = (int)pOpMutiple[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].waveSelect = (int)pOpLfoEg[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].vibEnable = pOpLfoPmEnable[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].amEnable = pOpLfoAmEnable[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].ams = pOpLfoAms[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].amd = pOpLfoAmd[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pms = pOpLfoPms[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pmd = pOpLfoPmd[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].mask = pOpMask[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;

        params.opl3.op[op].m_adsrParams.ar = (int)pOpAdsrAr[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].m_adsrParams.dr = (int)pOpAdsrDr[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].m_adsrParams.sl = (int)pOpAdsrSl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].m_adsrParams.rr = (int)pOpAdsrRr[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].m_adsrParams.tl = (int)pOpAdsrTl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].m_adsrParams.egType = pOpAdsrEgType[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].m_adsrParams.ksr = pOpAdsrKsr[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].m_adsrParams.ksl = (int)pOpAdsrKsl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].m_adsrParams.xof = pOpAdsrXof[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].m_adsrParams.kor = pOpAdsrKor[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].m_adsrParams.bypass = pOpAdsrBypass[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;

        params.opl3.op[op].pitchEnvEnable = pOpPitchEnvEnable[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].pitchAdsr.bypass = false;
        params.opl3.op[op].pitchAdsr.ar = pOpPitchEnvAr[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pitchAdsr.dr = pOpPitchEnvDr[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pitchAdsr.rr = pOpPitchEnvRr[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pitchAdsr.stl = (int)pOpPitchEnvStl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pitchAdsr.atl = (int)pOpPitchEnvAtl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pitchAdsr.ssl = (int)pOpPitchEnvSsl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].pitchAdsr.rll = (int)pOpPitchEnvRll[op]->load(std::memory_order_relaxed);

        params.opl3.op[op].ssgEnvEnable = pOpSsgSwEnvEnable[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].ssgSwEnv.bypass = false;
        params.opl3.op[op].ssgSwEnv.steps = (int)pOpSsgSwEnvSteps[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.loop = pOpSsgSwEnvLoop[op]->load(std::memory_order_relaxed) > Opl3PrValue::boolThread;
        params.opl3.op[op].ssgSwEnv.loopTo = (int)pOpSsgSwEnvLoopTo[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.loopCount = (int)pOpSsgSwEnvLoopCount[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.stl = pOpSsgSwEnvStl[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.r1 = pOpSsgSwEnvR1[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.l1 = pOpSsgSwEnvL1[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.r2 = pOpSsgSwEnvR2[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.l2 = pOpSsgSwEnvL2[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.r3 = pOpSsgSwEnvR3[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.l3 = pOpSsgSwEnvL3[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.r4 = pOpSsgSwEnvR4[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.l4 = pOpSsgSwEnvL4[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.r5 = pOpSsgSwEnvR5[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.l5 = pOpSsgSwEnvL5[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.r6 = pOpSsgSwEnvR6[op]->load(std::memory_order_relaxed);
        params.opl3.op[op].ssgSwEnv.l6 = pOpSsgSwEnvL6[op]->load(std::memory_order_relaxed);
    }
}
