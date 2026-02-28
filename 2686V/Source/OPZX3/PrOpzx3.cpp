#include "PrOpzx3.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void Opzx3Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opzx3;

    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opzx3::Alg::min, PrValue::Opzx3::Alg::max, PrValue::Opzx3::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Opzx3::Fb0::min, PrValue::Opzx3::Fb0::max, PrValue::Opzx3::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Opzx3::Bit::min, PrValue::Opzx3::Bit::max, PrValue::Opzx3::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Opzx3::Rate::min, PrValue::Opzx3::Rate::max, PrValue::Opzx3::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::freq, code + PrName::Fm::Post::Lfo::freq, PrValue::Opzx3::Lfo::Freq::min, PrValue::Opzx3::Lfo::Freq::max, PrValue::Opzx3::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pms, code + PrName::Fm::Post::Lfo::pms, PrValue::Opzx3::Lfo::Pms::min, PrValue::Opzx3::Lfo::Pms::max, PrValue::Opzx3::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::ams, code + PrName::Fm::Post::Lfo::ams, PrValue::Opzx3::Lfo::Ams::min, PrValue::Opzx3::Lfo::Ams::max, PrValue::Opzx3::Lfo::Ams::initial));

    for (int op = 0; op < PrValue::Opzx3::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Opzx3::Op::Mul::min, PrValue::Opzx3::Op::Mul::max, PrValue::Opzx3::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt, namePrefix + PrName::Fm::Op::Post::dt1, PrValue::Opzx3::Op::Dt1::min, PrValue::Opzx3::Op::Dt1::max, PrValue::Opzx3::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt2, namePrefix + PrName::Fm::Op::Post::dt2, PrValue::Opzx3::Op::Dt2::min, PrValue::Opzx3::Op::Dt2::max, PrValue::Opzx3::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Opzx3::Op::Ar::min, PrValue::Opzx3::Op::Ar::max, PrValue::Opzx3::Op::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d1r, namePrefix + PrName::Fm::Op::Post::d1r, PrValue::Opzx3::Op::D1r::min, PrValue::Opzx3::Op::D1r::max, PrValue::Opzx3::Op::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d1l, namePrefix + PrName::Fm::Op::Post::d1l, PrValue::Opzx3::Op::D1l::min, PrValue::Opzx3::Op::D1l::max, PrValue::Opzx3::Op::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d2r, namePrefix + PrName::Fm::Op::Post::d2r, PrValue::Opzx3::Op::D2r::min, PrValue::Opzx3::Op::D2r::max, PrValue::Opzx3::Op::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Opzx3::Op::Rr::min, PrValue::Opzx3::Op::Rr::max, PrValue::Opzx3::Op::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Opzx3::Op::Tl::min, PrValue::Opzx3::Op::Tl::max, PrValue::Opzx3::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::seFreq, namePrefix + PrName::Fm::Op::Post::seFreq, PrValue::Opzx3::Op::SeFreq::min, PrValue::Opzx3::Op::SeFreq::max, PrValue::Opzx3::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ks, namePrefix + PrName::Fm::Op::Post::ks, PrValue::Opzx3::Op::Ks::min, PrValue::Opzx3::Op::Ks::max, PrValue::Opzx3::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::fix, namePrefix + PrName::Fm::Op::Post::fix, PrValue::Opzx3::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::fixFreq, namePrefix + PrName::Fm::Op::Post::fixFreq, PrValue::Opzx3::Op::FixFreq::min, PrValue::Opzx3::Op::FixFreq::max, PrValue::Opzx3::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ws, namePrefix + PrName::Fm::Op::Post::ws, PrValue::Opzx3::Op::Ws::min, PrValue::Opzx3::Op::Ws::max, PrValue::Opzx3::Op::Ws::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Opzx3::Op::Mask::initial)); // OP Mask (Switch)
    }
}

void Opzx3Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opzx3;

    params.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);
    params.lfoFreq = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::freq);
    params.pms = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pms);
    params.ams = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::ams);

    for (int op = 0; op < PrValue::Opzx3::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt);
        params.fmOp[op].detune2 = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt2);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ar);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::d1r);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::d1l);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ks);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::tl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::d2r);
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::seFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fix) > PrValue::boolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fixFreq);
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ws);
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);
    }
}
