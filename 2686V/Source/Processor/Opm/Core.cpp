#include "./Core.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opm;

    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opm::Alg::min, PrValue::Opm::Alg::max, PrValue::Opm::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Fm::Fb0::min, PrValue::Fm::Fb0::max, PrValue::Fm::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::fb2, code + PrName::Fm::Post::fb2, PrValue::Fm::Fb2::min, PrValue::Fm::Fb2::max, PrValue::Fm::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::freq, code + PrName::Fm::Post::Lfo::freq, PrValue::Opm::Lfo::Freq::min, PrValue::Opm::Lfo::Freq::max, PrValue::Opm::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::amSmoothRatio, code + PrName::Fm::Post::Lfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pgShape, code + PrName::Fm::Post::Lfo::pgShape, PrValue::Lfo::PgShape::min, PrValue::Lfo::PgShape::max, PrValue::Lfo::PgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::egShape, code + PrName::Fm::Post::Lfo::egShape, PrValue::Lfo::EgShape::min, PrValue::Lfo::EgShape::max, PrValue::Lfo::EgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::am, code + PrName::Fm::Post::Lfo::am, PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::pm, code + PrName::Fm::Post::Lfo::pm, PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pms, code + PrName::Fm::Post::Lfo::pms, PrValue::Lfo::Pms::min, PrValue::Lfo::Pms::max, PrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::ams, code + PrName::Fm::Post::Lfo::ams, PrValue::Lfo::Ams::min, PrValue::Lfo::Ams::max, PrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pmd, code + PrName::Fm::Post::Lfo::pmd, PrValue::Lfo::Pmd::min, PrValue::Lfo::Pmd::max, PrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::amd, code + PrName::Fm::Post::Lfo::amd, PrValue::Lfo::Amd::min, PrValue::Lfo::Amd::max, PrValue::Lfo::Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::syncDelay, code + PrName::Fm::Post::Lfo::syncDelay, PrValue::Lfo::SyncDelay::min, PrValue::Lfo::SyncDelay::max, PrValue::Lfo::SyncDelay::initial));

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
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::amsEn, namePrefix + PrName::Fm::Op::Post::amsEn, PrValue::OpLfo::AmsEn::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::fix, namePrefix + PrName::Fm::Op::Post::fix, PrValue::Fix::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::fixFreq, namePrefix + PrName::Fm::Op::Post::fixFreq, PrValue::Fix::Freq::min, PrValue::Fix::Freq::max, PrValue::Fix::Freq::initial));
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

    params.opm.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);
    params.opm.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.opm.feedback2 = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb2);
    params.opm.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.opm.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);
    params.opm.lfoFreq = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::freq);
    params.opm.lfoWave = 0;
    params.opm.pgLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pgShape);
    params.opm.egLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::egShape);
    params.opm.lfoAmSmRt = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amSmoothRatio);
    params.opm.amEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::am) > PrValue::boolThread);
    params.opm.pmEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pm) > PrValue::boolThread);
    params.opm.lfoPms = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pms);
    params.opm.lfoAms = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::ams);
    params.opm.lfoPmd = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pmd);
    params.opm.lfoAmd = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amd);
    params.opm.lfoSyncDelay = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::syncDelay);

    for (int op = 0; op < PrValue::Opm::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.opm.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.opm.op[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt);
        params.opm.op[op].detune2 = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt2);
        params.opm.op[op].attack = 0.0f;
        params.opm.op[op].decay = 0.0f;
        params.opm.op[op].sustain = 0.0f;
        params.opm.op[op].release = 0.0f;
        params.opm.op[op].sustainRate = 0.0f;
        params.opm.op[op].totalLevel = 0.0f;
        params.opm.op[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ks);
        params.opm.op[op].keyScaleLevel = 0;
        params.opm.op[op].totalLevel = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::tl);
        params.opm.op[op].phaseOffset = 0.0f;
        params.opm.op[op].ssgEg = 0;
        params.opm.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::seFreq);
        params.opm.op[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fix) > PrValue::boolThread);
        params.opm.op[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fixFreq);
        params.opm.op[op].waveSelect = 0; // Sine
        params.opm.op[op].egType = true;
        params.opm.op[op].vibEnable = false;
        params.opm.op[op].pms = 0;
		params.opm.op[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::amsEn) > PrValue::boolThread); // AMS Enable を AM Enable で代用
        params.opm.op[op].ams = 0;
        params.opm.op[op].oplAms = 0.0f;
        params.opm.op[op].oplAmd = 0.0f;
        params.opm.op[op].oplPms = 0.0f;
        params.opm.op[op].oplPmd = 0.0f;
        params.opm.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);

        params.opm.op[op].isOplMode = false;
        params.opm.op[op].regEnable = true;
        params.opm.op[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgAr);
        params.opm.op[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD1r);
        params.opm.op[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD1l);
        params.opm.op[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD2r);
        params.opm.op[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgRr);
        params.opm.op[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgTl);
    }
}
