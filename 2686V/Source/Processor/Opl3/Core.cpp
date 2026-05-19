#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void Opl3Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = Opl3PrKey::prefix;

    // ==============================================================================
    // OPL3 Parameters (4 Ops, 8 Waveforms)
    // ==============================================================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::alg, code + Opl3PrName::alg, Opl3PrValue::Alg::min, Opl3PrValue::Alg::max, Opl3PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::fb, code + Opl3PrName::fb, Opl3PrValue::Fb::min, Opl3PrValue::Fb::max, Opl3PrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::bit, code + Opl3PrName::bit, Opl3PrValue::Bit::min, Opl3PrValue::Bit::max, Opl3PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::rate, code + Opl3PrName::rate, Opl3PrValue::Rate::min, Opl3PrValue::Rate::max, Opl3PrValue::Rate::initial)); // Default 6 (16kHz)

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

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgAr, namePrefix + Opl3PrName::rgAr, Opl3PrValue::Op::RgAdsr::Ar::min, Opl3PrValue::Op::RgAdsr::Ar::max, Opl3PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgDr, namePrefix + Opl3PrName::rgDr, Opl3PrValue::Op::RgAdsr::Dr::min, Opl3PrValue::Op::RgAdsr::Dr::max, Opl3PrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgSl, namePrefix + Opl3PrName::rgSl, Opl3PrValue::Op::RgAdsr::Sl::min, Opl3PrValue::Op::RgAdsr::Sl::max, Opl3PrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgRr, namePrefix + Opl3PrName::rgRr, Opl3PrValue::Op::RgAdsr::Rr::min, Opl3PrValue::Op::RgAdsr::Rr::max, Opl3PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opl3PrKey::rgTl, namePrefix + Opl3PrName::rgTl, Opl3PrValue::Op::RgAdsr::Tl::min, Opl3PrValue::Op::RgAdsr::Tl::max, Opl3PrValue::Op::RgAdsr::Tl::initial));
    }
}

void Opl3Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = Opl3PrKey::prefix;

    params.opl3.algorithm = (int)*apvts.getRawParameterValue(code + Opl3PrKey::alg);
    params.opl3.feedback = *apvts.getRawParameterValue(code + Opl3PrKey::fb);
    params.opl3.fmBitDepth = (int)*apvts.getRawParameterValue(code + Opl3PrKey::bit);
    params.opl3.fmRateIndex = (int)*apvts.getRawParameterValue(code + Opl3PrKey::rate);

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        juce::String p = code + Opl3PrKey::op + juce::String(op);

        params.opl3.op[op].multiple = (int)*apvts.getRawParameterValue(p + Opl3PrKey::mul);
        bool ksrOn = *apvts.getRawParameterValue(p + Opl3PrKey::ksr) > Opl3PrValue::boolThread;
        params.opl3.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + Opl3PrKey::eg);
        params.opl3.op[op].vibEnable = *apvts.getRawParameterValue(p + Opl3PrKey::vib) > Opl3PrValue::boolThread;
        params.opl3.op[op].amEnable = (*apvts.getRawParameterValue(p + Opl3PrKey::am) > Opl3PrValue::boolThread);
        params.opl3.op[op].ams = *apvts.getRawParameterValue(p + Opl3PrKey::ams);
        params.opl3.op[op].amd = *apvts.getRawParameterValue(p + Opl3PrKey::amd);
        params.opl3.op[op].pms = *apvts.getRawParameterValue(p + Opl3PrKey::pms);
        params.opl3.op[op].pmd = *apvts.getRawParameterValue(p + Opl3PrKey::pmd);
        params.opl3.op[op].mask = (*apvts.getRawParameterValue(p + Opl3PrKey::mask) > Opl3PrValue::boolThread);

        params.opl3.op[op].m_adsrParams.ar = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgAr);
        params.opl3.op[op].m_adsrParams.dr = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgDr);
        params.opl3.op[op].m_adsrParams.sl = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgSl);
        params.opl3.op[op].m_adsrParams.rr = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgRr);
        params.opl3.op[op].m_adsrParams.tl = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgTl);
        params.opl3.op[op].m_adsrParams.ksr = ksrOn;
        params.opl3.op[op].m_adsrParams.ksl = (int)*apvts.getRawParameterValue(p + Opl3PrKey::ksl);
        params.opl3.op[op].m_adsrParams.egType = *apvts.getRawParameterValue(p + Opl3PrKey::egType) > Opl3PrValue::boolThread;
    }
}
