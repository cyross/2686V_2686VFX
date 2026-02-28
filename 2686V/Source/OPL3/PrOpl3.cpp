#include "PrOpl3.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void Opl3Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opl3;

    // ==============================================================================
    // OPL3 Parameters (4 Ops, 8 Waveforms)
    // ==============================================================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opl3::Alg::min, PrValue::Opl3::Alg::max, PrValue::Opl3::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Opl3::Fb0::min, PrValue::Opl3::Fb0::max, PrValue::Opl3::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb2, code + PrName::Fm::Post::fb2, PrValue::Opl3::Fb2::min, PrValue::Opl3::Fb2::max, PrValue::Opl3::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Opl3::Bit::min, PrValue::Opl3::Bit::max, PrValue::Opl3::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Opl3::Rate::min, PrValue::Opl3::Rate::max, PrValue::Opl3::Rate::initial)); // Default 6 (16kHz)

    for (int i = 0; i < PrValue::Opl3::ops; ++i)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(i);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(i + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Opl3::Op::Mul::min, PrValue::Opl3::Op::Mul::max, PrValue::Opl3::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Opl3::Op::Tl::min, PrValue::Opl3::Op::Tl::max, PrValue::Opl3::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Opl3::Op::Ar::min, PrValue::Opl3::Op::Ar::max, PrValue::Opl3::Op::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::dr, namePrefix + PrName::Fm::Op::Post::dr, PrValue::Opl3::Op::Dr::min, PrValue::Opl3::Op::Dr::max, PrValue::Opl3::Op::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sl, namePrefix + PrName::Fm::Op::Post::sl, PrValue::Opl3::Op::Sl::min, PrValue::Opl3::Op::Sl::max, PrValue::Opl3::Op::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Opl3::Op::Rr::min, PrValue::Opl3::Op::Rr::max, PrValue::Opl3::Op::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::am, namePrefix + PrName::Fm::Op::Post::am, PrValue::Opl3::Op::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::vib, namePrefix + PrName::Fm::Op::Post::vib, PrValue::Opl3::Op::Vib::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::egType, namePrefix + PrName::Fm::Op::Post::egType, PrValue::Opl3::Op::EgType::initial)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::ksr, namePrefix + PrName::Fm::Op::Post::ksr, PrValue::Opl3::Op::Ksr::initial)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ksl, namePrefix + PrName::Fm::Op::Post::ksl, PrValue::Opl3::Op::Ksl::min, PrValue::Opl3::Op::Ksl::max, PrValue::Opl3::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::eg, namePrefix + PrName::Fm::Op::Post::eg, PrValue::Opl3::Op::Eg::min, PrValue::Opl3::Op::Eg::max, PrValue::Opl3::Op::Eg::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Opl3::Op::Mask::initial)); // OP Mask (Switch)
    }
}

void Opl3Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opl3;

    params.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.feedback2 = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);

    for (int op = 0; op < PrValue::Opl3::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.fmOp[op].detune = 0;
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
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::am) > PrValue::boolThread);
        params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::vib) > PrValue::boolThread;
        params.fmOp[op].egType = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::egType) > PrValue::boolThread;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);
    }
}
