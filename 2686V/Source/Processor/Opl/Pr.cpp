#include "./Pr.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opl;

    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opl::Alg::min, PrValue::Opl::Alg::max, PrValue::Opl::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Fm::Fb0::min, PrValue::Fm::Fb0::max, PrValue::Fm::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)

    for (int op = 0; op < PrValue::Opl::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Op::Tl::min, PrValue::Op::Tl::max, PrValue::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::dr, namePrefix + PrName::Fm::Op::Post::dr, PrValue::Adsr::Dr::min, PrValue::Adsr::Dr::max, PrValue::Adsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sl, namePrefix + PrName::Fm::Op::Post::sl, PrValue::Adsr::Sl::min, PrValue::Adsr::Sl::max, PrValue::Adsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::am, namePrefix + PrName::Fm::Op::Post::am, PrValue::OplOpLfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::vib, namePrefix + PrName::Fm::Op::Post::vib, PrValue::OplOpLfo::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ams, namePrefix + PrName::Fm::Op::Post::ams, PrValue::OplOpLfo::Ams::min, PrValue::OplOpLfo::Ams::max, PrValue::OplOpLfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::amd, namePrefix + PrName::Fm::Op::Post::amd, PrValue::OplOpLfo::Amd::min, PrValue::OplOpLfo::Amd::max, PrValue::OplOpLfo::Amd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pms, namePrefix + PrName::Fm::Op::Post::pms, PrValue::OplOpLfo::Pms::min, PrValue::OplOpLfo::Pms::max, PrValue::OplOpLfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pmd, namePrefix + PrName::Fm::Op::Post::pmd, PrValue::OplOpLfo::Pmd::min, PrValue::OplOpLfo::Pmd::max, PrValue::OplOpLfo::Pmd::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::egType, namePrefix + PrName::Fm::Op::Post::egType, PrValue::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::ksr, namePrefix + PrName::Fm::Op::Post::ksr, PrValue::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ksl, namePrefix + PrName::Fm::Op::Post::ksl, PrValue::Op::Ksl::min, PrValue::Op::Ksl::max, PrValue::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::eg, namePrefix + PrName::Fm::Op::Post::eg, PrValue::Opl::Op::Eg::min, PrValue::Opl::Op::Eg::max, PrValue::Opl::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::rgEn, namePrefix + PrName::Fm::Op::Post::rgEn, PrValue::OplRg::En::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgAr, namePrefix + PrName::Fm::Op::Post::rgAr, PrValue::OplRg::Ar::min, PrValue::OplRg::Ar::max, PrValue::OplRg::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgDr, namePrefix + PrName::Fm::Op::Post::rgDr, PrValue::OplRg::Dr::min, PrValue::OplRg::Dr::max, PrValue::OplRg::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgSl, namePrefix + PrName::Fm::Op::Post::rgSl, PrValue::OplRg::Sl::min, PrValue::OplRg::Sl::max, PrValue::OplRg::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgRr, namePrefix + PrName::Fm::Op::Post::rgRr, PrValue::OplRg::Rr::min, PrValue::OplRg::Rr::max, PrValue::OplRg::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgTl, namePrefix + PrName::Fm::Op::Post::rgTl, PrValue::OplRg::Tl::min, PrValue::OplRg::Tl::max, PrValue::OplRg::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::sus, namePrefix + PrName::Fm::Op::Post::sus, PrValue::Opl::Op::Sus::initial));
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opl;

    params.opl.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.opl.feedback0 = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.opl.bitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.opl.rateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);
    params.opl.lfoFreq = 0.0f;
    params.opl.amEnable = false;
    params.opl.pmEnable = false;
    params.opl.lfoPms = 0;
    params.opl.lfoAms = 0;
    params.opl.lfoPmd = 0;
    params.opl.lfoAmd = 0;

    for (int op = 0; op < PrValue::Opl::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.opl.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.opl.op[op].detune = 0;
        params.opl.op[op].ampEnv.ar = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ar);
        params.opl.op[op].ampEnv.dr = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dr);
        params.opl.op[op].ampEnv.sl = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::sl);
        params.opl.op[op].ampEnv.rr = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rr);
        params.opl.op[op].ampEnv.tl = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::tl);
        bool ksrOn = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ksr) > PrValue::boolThread;
        params.opl.op[op].keyScale = ksrOn ? 3 : 0;
        params.opl.op[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ksl);
        params.opl.op[op].phaseOffset = 0.0f;
        params.opl.op[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.opl.op[op].fmSsgEgFreq = 0.0f;
        params.opl.op[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.opl.op[op].fixedFreq = 0.0f;
        params.opl.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::eg);
        params.opl.op[op].egType = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::egType) > PrValue::boolThread;
        params.opl.op[op].vibEnable = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::vib) > PrValue::boolThread;
        params.opl.op[op].pms = 0;
        params.opl.op[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::am) > PrValue::boolThread);
        params.opl.op[op].ams = 0;
        params.opl.op[op].oplAms = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ams);
        params.opl.op[op].oplAmd = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::amd);
        params.opl.op[op].oplPms = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pms);
        params.opl.op[op].oplPmd = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pmd);
        params.opl.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);

        params.opl.op[op].regEnv.enable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgEn) > PrValue::boolThread);
        params.opl.op[op].regEnv.ar = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgAr);
        params.opl.op[op].regEnv.dr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgDr);
        params.opl.op[op].regEnv.sl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgSl);
        params.opl.op[op].regEnv.rr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgRr);
        params.opl.op[op].regEnv.tl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgTl);

        params.opl.op[op].susEnable = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::sus) > PrValue::boolThread;
    }
}
