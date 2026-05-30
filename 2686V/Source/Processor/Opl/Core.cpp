#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = OplPrKey::prefix;

    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OplPrKey::alg, code + OplPrName::alg, OplPrValue::Alg::min, OplPrValue::Alg::max, OplPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OplPrKey::fb, code + OplPrName::fb, OplPrValue::Fb::min, OplPrValue::Fb::max, OplPrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OplPrKey::bit, code + OplPrName::bit, OplPrValue::Bit::min, OplPrValue::Bit::max, OplPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OplPrKey::rate, code + OplPrName::rate, OplPrValue::Rate::min, OplPrValue::Rate::max, OplPrValue::Rate::initial)); // Default 6 (16kHz)

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String prefix = code + OplPrKey::op + juce::String(op);
        juce::String namePrefix = code + OplPrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::mul, namePrefix + OplPrName::mul, OplPrValue::Op::Mul::min, OplPrValue::Op::Mul::max, OplPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::am, namePrefix + OplPrName::am, OplPrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::vib, namePrefix + OplPrName::vib, OplPrValue::Op::Lfo::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OplPrKey::ams, namePrefix + OplPrName::ams, OplPrValue::Op::Lfo::Ams::min, OplPrValue::Op::Lfo::Ams::max, OplPrValue::Op::Lfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OplPrKey::amd, namePrefix + OplPrName::amd, OplPrValue::Op::Lfo::Amd::min, OplPrValue::Op::Lfo::Amd::max, OplPrValue::Op::Lfo::Amd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OplPrKey::pms, namePrefix + OplPrName::pms, OplPrValue::Op::Lfo::Pms::min, OplPrValue::Op::Lfo::Pms::max, OplPrValue::Op::Lfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OplPrKey::pmd, namePrefix + OplPrName::pmd, OplPrValue::Op::Lfo::Pmd::min, OplPrValue::Op::Lfo::Pmd::max, OplPrValue::Op::Lfo::Pmd::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::egType, namePrefix + OplPrName::egType, OplPrValue::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::ksr, namePrefix + OplPrName::ksr, OplPrValue::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::ksl, namePrefix + OplPrName::ksl, OplPrValue::Op::Ksl::min, OplPrValue::Op::Ksl::max, OplPrValue::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::eg, namePrefix + OplPrName::eg, OplPrValue::Op::Eg::min, OplPrValue::Op::Eg::max, OplPrValue::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::mask, namePrefix + OplPrName::mask, OplPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::PitchAdsr::enable, namePrefix + OplPrName::PitchAdsr::enable, OplPrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::SsgSwEnv::enable, namePrefix + OplPrName::SsgSwEnv::enable, OplPrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgAr, namePrefix + OplPrName::rgAr, OplPrValue::Op::RgAdsr::Ar::min, OplPrValue::Op::RgAdsr::Ar::max, OplPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgDr, namePrefix + OplPrName::rgDr, OplPrValue::Op::RgAdsr::Dr::min, OplPrValue::Op::RgAdsr::Dr::max, OplPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgSl, namePrefix + OplPrName::rgSl, OplPrValue::Op::RgAdsr::Sl::min, OplPrValue::Op::RgAdsr::Sl::max, OplPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgRr, namePrefix + OplPrName::rgRr, OplPrValue::Op::RgAdsr::Rr::min, OplPrValue::Op::RgAdsr::Rr::max, OplPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgTl, namePrefix + OplPrName::rgTl, OplPrValue::Op::RgAdsr::Tl::min, OplPrValue::Op::RgAdsr::Tl::max, OplPrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::sus, namePrefix + OplPrName::sus, OplPrValue::Op::Sus::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::xof, namePrefix + OplPrName::xof, OplPrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::ampBypass, namePrefix + OplPrName::ampBypass, OplPrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
        addOpSsgSwEnvParameters(layout, prefix, namePrefix);
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = OplPrKey::prefix;

    params.opl.algorithm = (int)*apvts.getRawParameterValue(code + OplPrKey::alg);
    params.opl.feedback = *apvts.getRawParameterValue(code + OplPrKey::fb);
    params.opl.fmBitDepth = (int)*apvts.getRawParameterValue(code + OplPrKey::bit);
    params.opl.fmRateIndex = (int)*apvts.getRawParameterValue(code + OplPrKey::rate);

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String p = code + OplPrKey::op + juce::String(op);

        params.opl.op[op].multiple = (int)*apvts.getRawParameterValue(p + OplPrKey::mul);
        bool ksrOn = *apvts.getRawParameterValue(p + OplPrKey::ksr) > OplPrValue::boolThread;
        params.opl.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + OplPrKey::eg);
        params.opl.op[op].vibEnable = *apvts.getRawParameterValue(p + OplPrKey::vib) > OplPrValue::boolThread;
        params.opl.op[op].amEnable = (*apvts.getRawParameterValue(p + OplPrKey::am) > OplPrValue::boolThread);
        params.opl.op[op].ams = *apvts.getRawParameterValue(p + OplPrKey::ams);
        params.opl.op[op].amd = *apvts.getRawParameterValue(p + OplPrKey::amd);
        params.opl.op[op].pms = *apvts.getRawParameterValue(p + OplPrKey::pms);
        params.opl.op[op].pmd = *apvts.getRawParameterValue(p + OplPrKey::pmd);
        params.opl.op[op].mask = (*apvts.getRawParameterValue(p + OplPrKey::mask) > OplPrValue::boolThread);

        params.opl.op[op].m_adsrParams.ar = (int)*apvts.getRawParameterValue(p + OplPrKey::rgAr);
        params.opl.op[op].m_adsrParams.dr = (int)*apvts.getRawParameterValue(p + OplPrKey::rgDr);
        params.opl.op[op].m_adsrParams.sl = (int)*apvts.getRawParameterValue(p + OplPrKey::rgSl);
        params.opl.op[op].m_adsrParams.rr = (int)*apvts.getRawParameterValue(p + OplPrKey::rgRr);
        params.opl.op[op].m_adsrParams.tl = (int)*apvts.getRawParameterValue(p + OplPrKey::rgTl);
        params.opl.op[op].m_adsrParams.egType = *apvts.getRawParameterValue(p + OplPrKey::egType) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.ksr = ksrOn;
        params.opl.op[op].m_adsrParams.ksl = (int)*apvts.getRawParameterValue(p + OplPrKey::ksl);
        params.opl.op[op].m_adsrParams.sus = *apvts.getRawParameterValue(p + OplPrKey::sus) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.xof = *apvts.getRawParameterValue(p + OplPrKey::xof) > OplPrValue::boolThread;
        params.opl.op[op].m_adsrParams.bypass = *apvts.getRawParameterValue(p + OplPrKey::ampBypass) > OplPrValue::boolThread;

        params.opl.op[op].pitchEnvEnable = (*apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::enable) > OplPrValue::boolThread);
        params.opl.op[op].pitchAdsr.bypass = false;
        params.opl.op[op].pitchAdsr.ar = *apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::ar);
        params.opl.op[op].pitchAdsr.dr = *apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::dr);
        params.opl.op[op].pitchAdsr.rr = *apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::rr);
        params.opl.op[op].pitchAdsr.stl = (int)*apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::stl);
        params.opl.op[op].pitchAdsr.atl = (int)*apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::atl);
        params.opl.op[op].pitchAdsr.ssl = (int)*apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::ssl);
        params.opl.op[op].pitchAdsr.rll = (int)*apvts.getRawParameterValue(p + OplPrKey::PitchAdsr::rll);

        params.opl.op[op].ssgEnvEnable = (*apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::enable) > OplPrValue::boolThread);
        params.opl.op[op].ssgSwEnv.bypass = false;
        params.opl.op[op].ssgSwEnv.steps = (int)*apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::steps);
        params.opl.op[op].ssgSwEnv.loop = (*apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::loop) > OplPrValue::boolThread);
        params.opl.op[op].ssgSwEnv.loopTo = (int)*apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::loopTo);
        params.opl.op[op].ssgSwEnv.loopCount = (int)*apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::loopCount);
        params.opl.op[op].ssgSwEnv.stl = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::stl);
        params.opl.op[op].ssgSwEnv.r1 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r1);
        params.opl.op[op].ssgSwEnv.l1 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l1);
        params.opl.op[op].ssgSwEnv.r2 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r2);
        params.opl.op[op].ssgSwEnv.l2 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l2);
        params.opl.op[op].ssgSwEnv.r3 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r3);
        params.opl.op[op].ssgSwEnv.l3 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l3);
        params.opl.op[op].ssgSwEnv.r4 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r4);
        params.opl.op[op].ssgSwEnv.l4 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l4);
        params.opl.op[op].ssgSwEnv.r5 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r5);
        params.opl.op[op].ssgSwEnv.l5 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l5);
        params.opl.op[op].ssgSwEnv.r6 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::r6);
        params.opl.op[op].ssgSwEnv.l6 = *apvts.getRawParameterValue(p + OplPrKey::SsgSwEnv::l6);
    }
}
