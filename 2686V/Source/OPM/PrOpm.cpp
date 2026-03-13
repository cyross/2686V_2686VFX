#include "PrOpm.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opm;

    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opm::Alg::min, PrValue::Opm::Alg::max, PrValue::Opm::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Fm::Fb0::min, PrValue::Fm::Fb0::max, PrValue::Fm::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb2, code + PrName::Fm::Post::fb2, PrValue::Fm::Fb2::min, PrValue::Fm::Fb2::max, PrValue::Fm::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::freq, code + PrName::Fm::Post::Lfo::freq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::shape, code + PrName::Fm::Post::Lfo::shape, PrValue::Lfo::Shape::min, PrValue::Lfo::Shape::max, PrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::amSmoothRatio, code + PrName::Fm::Post::Lfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::am, code + PrName::Fm::Post::Lfo::am, PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::pm, code + PrName::Fm::Post::Lfo::pm, PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pms, code + PrName::Fm::Post::Lfo::pms, PrValue::Lfo::Pms::min, PrValue::Lfo::Pms::max, PrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::ams, code + PrName::Fm::Post::Lfo::ams, PrValue::Lfo::Ams::min, PrValue::Lfo::Ams::max, PrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pmd, code + PrName::Fm::Post::Lfo::pmd, PrValue::Lfo::Pmd::min, PrValue::Lfo::Pmd::max, PrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::amd, code + PrName::Fm::Post::Lfo::amd, PrValue::Lfo::Amd::min, PrValue::Lfo::Amd::max, PrValue::Lfo::Amd::initial));

    for (int op = 0; op < PrValue::Opm::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt, namePrefix + PrName::Fm::Op::Post::dt1, PrValue::Op::Dt1::min, PrValue::Op::Dt1::max, PrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt2, namePrefix + PrName::Fm::Op::Post::dt2, PrValue::Op::Dt2::min, PrValue::Op::Dt2::max, PrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Op::Tl::min, PrValue::Op::Tl::max, PrValue::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d1r, namePrefix + PrName::Fm::Op::Post::d1r, PrValue::Adsr::D1r::min, PrValue::Adsr::D1r::max, PrValue::Adsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d1l, namePrefix + PrName::Fm::Op::Post::d1l, PrValue::Adsr::D1l::min, PrValue::Adsr::D1l::max, PrValue::Adsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d2r, namePrefix + PrName::Fm::Op::Post::d2r, PrValue::Adsr::D2r::min, PrValue::Adsr::D2r::max, PrValue::Adsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::seFreq, namePrefix + PrName::Fm::Op::Post::seFreq, PrValue::Op::SeFreq::min, PrValue::Op::SeFreq::max, PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ks, namePrefix + PrName::Fm::Op::Post::ks, PrValue::Op::Ks::min, PrValue::Op::Ks::max, PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::fix, namePrefix + PrName::Fm::Op::Post::fix, PrValue::Fix::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::fixFreq, namePrefix + PrName::Fm::Op::Post::fixFreq, PrValue::Fix::Freq::min, PrValue::Fix::Freq::max, PrValue::Fix::Freq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::am, namePrefix + PrName::Fm::Op::Post::am, PrValue::OpLfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::vib, namePrefix + PrName::Fm::Op::Post::vib, PrValue::OpLfo::Vib::initial)); // PM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::pms, namePrefix + PrName::Fm::Op::Post::pms, PrValue::OpLfo::Pms::min, PrValue::OpLfo::Pms::max, PrValue::OpLfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ams, namePrefix + PrName::Fm::Op::Post::ams, PrValue::OpLfo::Ams::min, PrValue::OpLfo::Ams::max, PrValue::OpLfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::rgEn, namePrefix + PrName::Fm::Op::Post::rgEn, PrValue::FmRg::En::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgAr, namePrefix + PrName::Fm::Op::Post::rgAr, PrValue::FmRg::Ar::min, PrValue::FmRg::Ar::max, PrValue::FmRg::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgD1r, namePrefix + PrName::Fm::Op::Post::rgD2r, PrValue::FmRg::D1r::min, PrValue::FmRg::D1r::max, PrValue::FmRg::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgD1l, namePrefix + PrName::Fm::Op::Post::rgD1l, PrValue::FmRg::D1l::min, PrValue::FmRg::D1l::max, PrValue::FmRg::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgD2r, namePrefix + PrName::Fm::Op::Post::rgD2r, PrValue::FmRg::D2r::min, PrValue::FmRg::D2r::max, PrValue::FmRg::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgRr, namePrefix + PrName::Fm::Op::Post::rgRr, PrValue::FmRg::Rr::min, PrValue::FmRg::Rr::max, PrValue::FmRg::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgTl, namePrefix + PrName::Fm::Op::Post::rgTl, PrValue::FmRg::Tl::min, PrValue::FmRg::Tl::max, PrValue::FmRg::Tl::initial));
    }
}

void OpmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opm;

    params.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.feedback2 = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);
    params.lfoFreq = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::freq);
    params.lfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::shape);
    params.lfoAmSmRt = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amSmoothRatio);
    params.amEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::am) > PrValue::boolThread);
    params.pmEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pm) > PrValue::boolThread);
    params.lfoPms = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pms);
    params.lfoAms = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::ams);
    params.lfoPmd = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pmd);
    params.lfoAmd = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amd);

    for (int op = 0; op < PrValue::Opm::ops; ++op)
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
        params.fmOp[op].phaseOffset = 0.0f;
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::seFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fix) > PrValue::boolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].egType = true;
        params.fmOp[op].vibEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::vib) > PrValue::boolThread);
        params.fmOp[op].pms = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pms);
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::am) > PrValue::boolThread);
        params.fmOp[op].ams = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ams);
        params.fmOp[op].oplAms = 0.0f;
        params.fmOp[op].oplAmd = 0.0f;
        params.fmOp[op].oplPms = 0.0f;
        params.fmOp[op].oplPmd = 0.0f;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);

        params.fmOp[op].isOplMode = false;
        params.fmOp[op].regEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgEn) > PrValue::boolThread);
        params.fmOp[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgAr);
        params.fmOp[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD1r);
        params.fmOp[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD1l);
        params.fmOp[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD2r);
        params.fmOp[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgRr);
        params.fmOp[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgTl);
    }
}
