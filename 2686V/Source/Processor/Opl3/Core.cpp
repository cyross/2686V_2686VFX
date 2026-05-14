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
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::fb0, code + Opl3PrName::fb0, Opl3PrValue::Fb0::min, Opl3PrValue::Fb0::max, Opl3PrValue::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opl3PrKey::fb2, code + Opl3PrName::fb2, Opl3PrValue::Fb2::min, Opl3PrValue::Fb2::max, Opl3PrValue::Fb2::initial));
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
    params.opl3.feedback = *apvts.getRawParameterValue(code + Opl3PrKey::fb0);
    params.opl3.feedback2 = *apvts.getRawParameterValue(code + Opl3PrKey::fb2);
    params.opl3.fmBitDepth = (int)*apvts.getRawParameterValue(code + Opl3PrKey::bit);
    params.opl3.fmRateIndex = (int)*apvts.getRawParameterValue(code + Opl3PrKey::rate);
    params.opl3.lfoFreq = 0.0f;
    params.opl3.amEnable = false;
    params.opl3.pmEnable = false;
    params.opl3.lfoPms = 0;
    params.opl3.lfoAms = 0;
    params.opl3.lfoPmd = 0;
    params.opl3.lfoAmd = 0;

    for (int op = 0; op < Opl3PrValue::ops; ++op)
    {
        juce::String p = code + Opl3PrKey::op + juce::String(op);

        params.opl3.op[op].multiple = (int)*apvts.getRawParameterValue(p + Opl3PrKey::mul);
        params.opl3.op[op].detune = 0;
        params.opl3.op[op].attack = 0.0f;
        params.opl3.op[op].decay = 0.0f;
        params.opl3.op[op].sustain = 0.0f;
        params.opl3.op[op].release = 0.0f;
        params.opl3.op[op].sustainRate = 0.0f;
        params.opl3.op[op].totalLevel = 0.0f;
        bool ksrOn = *apvts.getRawParameterValue(p + Opl3PrKey::ksr) > Opl3PrValue::boolThread;
        params.opl3.op[op].keyScale = ksrOn ? 3 : 0;
        params.opl3.op[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + Opl3PrKey::ksl);
        params.opl3.op[op].phaseOffset = 0.0f;
        params.opl3.op[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.opl3.op[op].fmSsgEgFreq = 0.0f;
        params.opl3.op[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.opl3.op[op].fixedFreq = 0.0f;
        params.opl3.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + Opl3PrKey::eg);
        params.opl3.op[op].egType = *apvts.getRawParameterValue(p + Opl3PrKey::egType) > Opl3PrValue::boolThread;
        params.opl3.op[op].vibEnable = *apvts.getRawParameterValue(p + Opl3PrKey::vib) > Opl3PrValue::boolThread;
        params.opl3.op[op].pms = 0;
        params.opl3.op[op].amEnable = (*apvts.getRawParameterValue(p + Opl3PrKey::am) > Opl3PrValue::boolThread);
        params.opl3.op[op].ams = 0;
        params.opl3.op[op].oplAms = *apvts.getRawParameterValue(p + Opl3PrKey::ams);
        params.opl3.op[op].oplAmd = *apvts.getRawParameterValue(p + Opl3PrKey::amd);
        params.opl3.op[op].oplPms = *apvts.getRawParameterValue(p + Opl3PrKey::pms);
        params.opl3.op[op].oplPmd = *apvts.getRawParameterValue(p + Opl3PrKey::pmd);
        params.opl3.op[op].mask = (*apvts.getRawParameterValue(p + Opl3PrKey::mask) > Opl3PrValue::boolThread);

        params.opl3.op[op].isOplMode = true;
        params.opl3.op[op].regEnable = true;
        params.opl3.op[op].rar = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgAr);
        params.opl3.op[op].rdr = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgDr);
        params.opl3.op[op].rsl = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgSl);
        params.opl3.op[op].rrr = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgRr);
        params.opl3.op[op].rtl = (int)*apvts.getRawParameterValue(p + Opl3PrKey::rgTl);
    }
}
