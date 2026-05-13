#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::alg, code + PrName::alg, PrValue::Alg::min, PrValue::Alg::max, PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb0, code + PrName::fb0, PrValue::Fb::min, PrValue::Fb::max, PrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::bit, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial)); // Default 6 (16kHz)

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String prefix = code + PrKey::op + juce::String(op);
        juce::String namePrefix = code + PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::mul, namePrefix + PrName::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::am, namePrefix + PrName::am, PrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::vib, namePrefix + PrName::vib, PrValue::Op::Lfo::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::ams, namePrefix + PrName::ams, PrValue::Op::Lfo::Ams::min, PrValue::Op::Lfo::Ams::max, PrValue::Op::Lfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::amd, namePrefix + PrName::amd, PrValue::Op::Lfo::Amd::min, PrValue::Op::Lfo::Amd::max, PrValue::Op::Lfo::Amd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::pms, namePrefix + PrName::pms, PrValue::Op::Lfo::Pms::min, PrValue::Op::Lfo::Pms::max, PrValue::Op::Lfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::pmd, namePrefix + PrName::pmd, PrValue::Op::Lfo::Pmd::min, PrValue::Op::Lfo::Pmd::max, PrValue::Op::Lfo::Pmd::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::egType, namePrefix + PrName::egType, PrValue::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::ksr, namePrefix + PrName::ksr, PrValue::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::ksl, namePrefix + PrName::ksl, PrValue::Op::Ksl::min, PrValue::Op::Ksl::max, PrValue::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::eg, namePrefix + PrName::eg, PrValue::Op::Eg::min, PrValue::Op::Eg::max, PrValue::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::mask, namePrefix + PrName::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgAr, namePrefix + PrName::rgAr, PrValue::Op::RgAdsr::Ar::min, PrValue::Op::RgAdsr::Ar::max, PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgDr, namePrefix + PrName::rgDr, PrValue::Op::RgAdsr::Dr::min, PrValue::Op::RgAdsr::Dr::max, PrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgSl, namePrefix + PrName::rgSl, PrValue::Op::RgAdsr::Sl::min, PrValue::Op::RgAdsr::Sl::max, PrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgRr, namePrefix + PrName::rgRr, PrValue::Op::RgAdsr::Rr::min, PrValue::Op::RgAdsr::Rr::max, PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgTl, namePrefix + PrName::rgTl, PrValue::Op::RgAdsr::Tl::min, PrValue::Op::RgAdsr::Tl::max, PrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::sus, namePrefix + PrName::sus, PrValue::Op::Sus::initial));
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    params.opl.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::alg);
    params.opl.feedback = *apvts.getRawParameterValue(code + PrKey::fb0);
    params.opl.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::bit);
    params.opl.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);
    params.opl.lfoFreq = 0.0f;
    params.opl.amEnable = false;
    params.opl.pmEnable = false;
    params.opl.lfoPms = 0;
    params.opl.lfoAms = 0;
    params.opl.lfoPmd = 0;
    params.opl.lfoAmd = 0;

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String p = code + PrKey::op + juce::String(op);

        params.opl.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::mul);
        params.opl.op[op].detune = 0;
        params.opl.op[op].attack = 0.0f;
        params.opl.op[op].decay = 0.0f;
        params.opl.op[op].sustain = 0.0f;
        params.opl.op[op].release = 0.0f;
        params.opl.op[op].sustainRate = 0.0f;
        params.opl.op[op].totalLevel = 0.0f;
        bool ksrOn = *apvts.getRawParameterValue(p + PrKey::ksr) > PrValue::boolThread;
        params.opl.op[op].keyScale = ksrOn ? 3 : 0;
        params.opl.op[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + PrKey::ksl);
        params.opl.op[op].phaseOffset = 0.0f;
        params.opl.op[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.opl.op[op].fmSsgEgFreq = 0.0f;
        params.opl.op[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.opl.op[op].fixedFreq = 0.0f;
        params.opl.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + PrKey::eg);
        params.opl.op[op].egType = *apvts.getRawParameterValue(p + PrKey::egType) > PrValue::boolThread;
        params.opl.op[op].vibEnable = *apvts.getRawParameterValue(p + PrKey::vib) > PrValue::boolThread;
        params.opl.op[op].pms = 0;
        params.opl.op[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::am) > PrValue::boolThread);
        params.opl.op[op].ams = 0;
        params.opl.op[op].oplAms = *apvts.getRawParameterValue(p + PrKey::ams);
        params.opl.op[op].oplAmd = *apvts.getRawParameterValue(p + PrKey::amd);
        params.opl.op[op].oplPms = *apvts.getRawParameterValue(p + PrKey::pms);
        params.opl.op[op].oplPmd = *apvts.getRawParameterValue(p + PrKey::pmd);
        params.opl.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::mask) > PrValue::boolThread);

        params.opl.op[op].isOplMode = true;
        params.opl.op[op].regEnable = true;
        params.opl.op[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::rgAr);
        params.opl.op[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::rgDr);
        params.opl.op[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::rgSl);
        params.opl.op[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::rgRr);
        params.opl.op[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::rgTl);

        params.opl.op[op].susEnable = *apvts.getRawParameterValue(p + PrKey::sus) > PrValue::boolThread;
    }
}
