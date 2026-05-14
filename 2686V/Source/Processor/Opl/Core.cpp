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
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OplPrKey::fb0, code + OplPrName::fb0, OplPrValue::Fb::min, OplPrValue::Fb::max, OplPrValue::Fb::initial));
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

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgAr, namePrefix + OplPrName::rgAr, OplPrValue::Op::RgAdsr::Ar::min, OplPrValue::Op::RgAdsr::Ar::max, OplPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgDr, namePrefix + OplPrName::rgDr, OplPrValue::Op::RgAdsr::Dr::min, OplPrValue::Op::RgAdsr::Dr::max, OplPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgSl, namePrefix + OplPrName::rgSl, OplPrValue::Op::RgAdsr::Sl::min, OplPrValue::Op::RgAdsr::Sl::max, OplPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgRr, namePrefix + OplPrName::rgRr, OplPrValue::Op::RgAdsr::Rr::min, OplPrValue::Op::RgAdsr::Rr::max, OplPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OplPrKey::rgTl, namePrefix + OplPrName::rgTl, OplPrValue::Op::RgAdsr::Tl::min, OplPrValue::Op::RgAdsr::Tl::max, OplPrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OplPrKey::sus, namePrefix + OplPrName::sus, OplPrValue::Op::Sus::initial));
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = OplPrKey::prefix;

    params.opl.algorithm = (int)*apvts.getRawParameterValue(code + OplPrKey::alg);
    params.opl.feedback = *apvts.getRawParameterValue(code + OplPrKey::fb0);
    params.opl.fmBitDepth = (int)*apvts.getRawParameterValue(code + OplPrKey::bit);
    params.opl.fmRateIndex = (int)*apvts.getRawParameterValue(code + OplPrKey::rate);
    params.opl.lfoFreq = 0.0f;
    params.opl.amEnable = false;
    params.opl.pmEnable = false;
    params.opl.lfoPms = 0;
    params.opl.lfoAms = 0;
    params.opl.lfoPmd = 0;
    params.opl.lfoAmd = 0;

    for (int op = 0; op < OplPrValue::ops; ++op)
    {
        juce::String p = code + OplPrKey::op + juce::String(op);

        params.opl.op[op].multiple = (int)*apvts.getRawParameterValue(p + OplPrKey::mul);
        params.opl.op[op].detune = 0;
        params.opl.op[op].attack = 0.0f;
        params.opl.op[op].decay = 0.0f;
        params.opl.op[op].sustain = 0.0f;
        params.opl.op[op].release = 0.0f;
        params.opl.op[op].sustainRate = 0.0f;
        params.opl.op[op].totalLevel = 0.0f;
        bool ksrOn = *apvts.getRawParameterValue(p + OplPrKey::ksr) > OplPrValue::boolThread;
        params.opl.op[op].keyScale = ksrOn ? 3 : 0;
        params.opl.op[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + OplPrKey::ksl);
        params.opl.op[op].phaseOffset = 0.0f;
        params.opl.op[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.opl.op[op].fmSsgEgFreq = 0.0f;
        params.opl.op[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.opl.op[op].fixedFreq = 0.0f;
        params.opl.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + OplPrKey::eg);
        params.opl.op[op].egType = *apvts.getRawParameterValue(p + OplPrKey::egType) > OplPrValue::boolThread;
        params.opl.op[op].vibEnable = *apvts.getRawParameterValue(p + OplPrKey::vib) > OplPrValue::boolThread;
        params.opl.op[op].pms = 0;
        params.opl.op[op].amEnable = (*apvts.getRawParameterValue(p + OplPrKey::am) > OplPrValue::boolThread);
        params.opl.op[op].ams = 0;
        params.opl.op[op].oplAms = *apvts.getRawParameterValue(p + OplPrKey::ams);
        params.opl.op[op].oplAmd = *apvts.getRawParameterValue(p + OplPrKey::amd);
        params.opl.op[op].oplPms = *apvts.getRawParameterValue(p + OplPrKey::pms);
        params.opl.op[op].oplPmd = *apvts.getRawParameterValue(p + OplPrKey::pmd);
        params.opl.op[op].mask = (*apvts.getRawParameterValue(p + OplPrKey::mask) > OplPrValue::boolThread);

        params.opl.op[op].isOplMode = true;
        params.opl.op[op].regEnable = true;
        params.opl.op[op].rar = (int)*apvts.getRawParameterValue(p + OplPrKey::rgAr);
        params.opl.op[op].rdr = (int)*apvts.getRawParameterValue(p + OplPrKey::rgDr);
        params.opl.op[op].rsl = (int)*apvts.getRawParameterValue(p + OplPrKey::rgSl);
        params.opl.op[op].rrr = (int)*apvts.getRawParameterValue(p + OplPrKey::rgRr);
        params.opl.op[op].rtl = (int)*apvts.getRawParameterValue(p + OplPrKey::rgTl);

        params.opl.op[op].susEnable = *apvts.getRawParameterValue(p + OplPrKey::sus) > OplPrValue::boolThread;
    }
}
