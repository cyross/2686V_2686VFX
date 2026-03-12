#include "PrOpn.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opn;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opn::Alg::min, PrValue::Opn::Alg::max, PrValue::Opn::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Opn::Fb0::min, PrValue::Opn::Fb0::max, PrValue::Opn::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb2, code + PrName::Fm::Post::fb2, PrValue::Opn::Fb2::min, PrValue::Opn::Fb2::max, PrValue::Opn::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Opn::Bit::min, PrValue::Opn::Bit::max, PrValue::Opn::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Opn::Rate::min, PrValue::Opn::Rate::max, PrValue::Opn::Rate::initial)); // Default 6 (16kHz)

    for (int op = 0; op < PrValue::Opn::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Opn::Op::Mul::min, PrValue::Opn::Op::Mul::max, PrValue::Opn::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt, namePrefix + PrName::Fm::Op::Post::dt, PrValue::Opn::Op::Dt::min, PrValue::Opn::Op::Dt::max, PrValue::Opn::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Opn::Op::Ar::min, PrValue::Opn::Op::Ar::max, PrValue::Opn::Op::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::dr, namePrefix + PrName::Fm::Op::Post::dr, PrValue::Opn::Op::Dr::min, PrValue::Opn::Op::Dr::max, PrValue::Opn::Op::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sr, namePrefix + PrName::Fm::Op::Post::sr, PrValue::Opn::Op::Sr::min, PrValue::Opn::Op::Sr::max, PrValue::Opn::Op::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sl, namePrefix + PrName::Fm::Op::Post::sl, PrValue::Opn::Op::Sl::min, PrValue::Opn::Op::Sl::max, PrValue::Opn::Op::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Opn::Op::Rr::min, PrValue::Opn::Op::Rr::max, PrValue::Opn::Op::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Opn::Op::Tl::min, PrValue::Opn::Op::Tl::max, PrValue::Opn::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ks, namePrefix + PrName::Fm::Op::Post::ks, PrValue::Opn::Op::Ks::min, PrValue::Opn::Op::Ks::max, PrValue::Opn::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::fix, namePrefix + PrName::Fm::Op::Post::fix, PrValue::Opn::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::fixFreq, namePrefix + PrName::Fm::Op::Post::fixFreq, PrValue::Opn::Op::FixFreq::min, PrValue::Opn::Op::FixFreq::max, PrValue::Opn::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Opn::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::rgEn, namePrefix + PrName::Fm::Op::Post::rgEn, PrValue::Opn::Op::RgEn::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgAr, namePrefix + PrName::Fm::Op::Post::rgAr, PrValue::Opn::Op::RgAr::min, PrValue::Opn::Op::RgAr::max, PrValue::Opn::Op::RgAr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgDr, namePrefix + PrName::Fm::Op::Post::rgDr, PrValue::Opn::Op::RgDr::min, PrValue::Opn::Op::RgDr::max, PrValue::Opn::Op::RgDr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgSr, namePrefix + PrName::Fm::Op::Post::rgSr, PrValue::Opn::Op::RgSr::min, PrValue::Opn::Op::RgSr::max, PrValue::Opn::Op::RgSr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgSl, namePrefix + PrName::Fm::Op::Post::rgSl, PrValue::Opn::Op::RgSl::min, PrValue::Opn::Op::RgSl::max, PrValue::Opn::Op::RgSl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgRr, namePrefix + PrName::Fm::Op::Post::rgRr, PrValue::Opn::Op::RgRr::min, PrValue::Opn::Op::RgRr::max, PrValue::Opn::Op::RgRr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgTl, namePrefix + PrName::Fm::Op::Post::rgTl, PrValue::Opn::Op::RgTl::min, PrValue::Opn::Op::RgTl::max, PrValue::Opn::Op::RgTl::initial));
    }
}

void OpnProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opn;

    params.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.feedback2 = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);
    params.lfoFreq = 0.0f;
    params.amEnable = false;
    params.pmEnable = false;
    params.lfoPms = 0;
    params.lfoAms = 0;
    params.lfoPmd = 0;
    params.lfoAmd = 0;

    for (int op = 0; op < PrValue::Opn::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ar);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::sl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ks);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::tl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::sr);
        params.fmOp[op].phaseOffset = 0.0f;
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = 0.0f;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fix) > PrValue::boolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].vibEnable = false;
        params.fmOp[op].pms = 0.0f;
        params.fmOp[op].amEnable = false; // OPNにはAMは無い
        params.fmOp[op].ams = 0.0f;
        params.fmOp[op].oplAms = 0.0f;
        params.fmOp[op].oplAmd = 0.0f;
        params.fmOp[op].oplPms = 0.0f;
        params.fmOp[op].oplPmd = 0.0f;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);

        params.fmOp[op].isOplMode = false;
        params.fmOp[op].regEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgEn) > PrValue::boolThread);
        params.fmOp[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgAr);
        params.fmOp[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgDr);
        params.fmOp[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgSl);
        params.fmOp[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgSr);
        params.fmOp[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgRr);
        params.fmOp[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgTl);
    }
}
