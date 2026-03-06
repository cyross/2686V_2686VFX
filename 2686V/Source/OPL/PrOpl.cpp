#include "PrOpl.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void OplProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opl;

    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opl::Alg::min, PrValue::Opl::Alg::max, PrValue::Opl::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Opl::Fb0::min, PrValue::Opl::Fb0::max, PrValue::Opl::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Opl::Bit::min, PrValue::Opl::Bit::max, PrValue::Opl::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Opl::Rate::min, PrValue::Opl::Rate::max, PrValue::Opl::Rate::initial)); // Default 6 (16kHz)

    for (int op = 0; op < PrValue::Opl::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Opl::Op::Mul::min, PrValue::Opl::Op::Mul::max, PrValue::Opl::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt, namePrefix + PrName::Fm::Op::Post::dt, PrValue::Opl::Op::Dt::min, PrValue::Opl::Op::Dt::max, PrValue::Opl::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Opl::Op::Tl::min, PrValue::Opl::Op::Tl::max, PrValue::Opl::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Opl::Op::Ar::min, PrValue::Opl::Op::Ar::max, PrValue::Opl::Op::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::dr, namePrefix + PrName::Fm::Op::Post::dr, PrValue::Opl::Op::Dr::min, PrValue::Opl::Op::Dr::max, PrValue::Opl::Op::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sl, namePrefix + PrName::Fm::Op::Post::sl, PrValue::Opl::Op::Sl::min, PrValue::Opl::Op::Sl::max, PrValue::Opl::Op::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Opl::Op::Rr::min, PrValue::Opl::Op::Rr::max, PrValue::Opl::Op::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::am, namePrefix + PrName::Fm::Op::Post::am, PrValue::Opl::Op::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::vib, namePrefix + PrName::Fm::Op::Post::vib, PrValue::Opl::Op::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ams, namePrefix + PrName::Fm::Op::Post::ams, PrValue::Opl::Op::Ams::min, PrValue::Opl::Op::Ams::max, PrValue::Opl::Op::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::amd, namePrefix + PrName::Fm::Op::Post::amd, PrValue::Opl::Op::Amd::min, PrValue::Opl::Op::Amd::max, PrValue::Opl::Op::Amd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pms, namePrefix + PrName::Fm::Op::Post::pms, PrValue::Opl::Op::Pms::min, PrValue::Opl::Op::Pms::max, PrValue::Opl::Op::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pmd, namePrefix + PrName::Fm::Op::Post::pmd, PrValue::Opl::Op::Pmd::min, PrValue::Opl::Op::Pmd::max, PrValue::Opl::Op::Pmd::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::egType, namePrefix + PrName::Fm::Op::Post::egType, PrValue::Opl::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::ksr, namePrefix + PrName::Fm::Op::Post::ksr, PrValue::Opl::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ksl, namePrefix + PrName::Fm::Op::Post::ksl, PrValue::Opl::Op::Ksl::min, PrValue::Opl::Op::Ksl::max, PrValue::Opl::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::eg, namePrefix + PrName::Fm::Op::Post::eg, PrValue::Opl::Op::Eg::min, PrValue::Opl::Op::Eg::max, PrValue::Opl::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Opl::Op::Mask::initial)); // OP Mask (Switch)
    }
}

void OplProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opl;

    params.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);
    params.lfoFreq = 0.0f;
    params.amEnable = false;
    params.pmEnable = false;
    params.lfoPms = 0;
    params.lfoAms = 0;
    params.lfoPmd = 0;
    params.lfoAmd = 0;

    for (int op = 0; op < PrValue::Opl::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ar);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::sl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rr);
        bool ksrOn = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ksr) > PrValue::boolThread;
        params.fmOp[op].keyScale = ksrOn ? 3 : 0;
        params.fmOp[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ksl);
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::tl);
        params.fmOp[op].sustainRate = 0.0f;
        params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.fmOp[op].fmSsgEgFreq = 0.0f;
        params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.fmOp[op].fixedFreq = 0.0f;
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::eg);
        params.fmOp[op].egType = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::egType) > PrValue::boolThread;
        params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::vib) > PrValue::boolThread;
        params.fmOp[op].pms = 0;
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::am) > PrValue::boolThread);
        params.fmOp[op].ams = 0;
        params.fmOp[op].oplAms = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ams);
        params.fmOp[op].oplAmd = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::amd);
        params.fmOp[op].oplPms = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pms);
        params.fmOp[op].oplPmd = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pmd);
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);
    }
}
