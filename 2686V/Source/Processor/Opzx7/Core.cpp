#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void Opzx7Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = Opzx7PrKey::prefix;

    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::alg, code + Opzx7PrName::alg, Opzx7PrValue::Alg::min, Opzx7PrValue::Alg::max, Opzx7PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::fb0, code + Opzx7PrName::fb0, Opzx7PrValue::Fb0::min, Opzx7PrValue::Fb0::max, Opzx7PrValue::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::fb2, code + Opzx7PrName::fb2, Opzx7PrValue::Fb2::min, Opzx7PrValue::Fb2::max, Opzx7PrValue::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::bit, code + Opzx7PrName::bit, Opzx7PrValue::Bit::min, Opzx7PrValue::Bit::max, Opzx7PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::rate, code + Opzx7PrName::rate, Opzx7PrValue::Rate::min, Opzx7PrValue::Rate::max, Opzx7PrValue::Rate::initial)); // Default 6 (16kHz)

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::GlLfo::freq, code + Opzx7PrName::GlLfo::freq, Opzx7PrValue::Lfo::Freq::min, Opzx7PrValue::Lfo::Freq::max, Opzx7PrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::GlLfo::syncDelay, code + Opzx7PrName::GlLfo::syncDelay, Opzx7PrValue::Lfo::SyncDelay::min, Opzx7PrValue::Lfo::SyncDelay::max, Opzx7PrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::GlLfo::amSmoothRatio, code + Opzx7PrName::GlLfo::amSmoothRatio, Opzx7PrValue::Lfo::AmSmRt::min, Opzx7PrValue::Lfo::AmSmRt::max, Opzx7PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::GlLfo::pgShape, code + Opzx7PrName::GlLfo::pgShape, Opzx7PrValue::Lfo::Shape::min, Opzx7PrValue::Lfo::Shape::max, Opzx7PrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::GlLfo::egShape, code + Opzx7PrName::GlLfo::egShape, Opzx7PrValue::Lfo::Shape::min, Opzx7PrValue::Lfo::Shape::max, Opzx7PrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Opzx7PrKey::GlLfo::am, code + Opzx7PrName::GlLfo::am, Opzx7PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Opzx7PrKey::GlLfo::pm, code + Opzx7PrName::GlLfo::pm, Opzx7PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::GlLfo::pms, code + Opzx7PrName::GlLfo::pms, Opzx7PrValue::Lfo::PmsFloat::min, Opzx7PrValue::Lfo::PmsFloat::max, Opzx7PrValue::Lfo::PmsFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::GlLfo::ams, code + Opzx7PrName::GlLfo::ams, Opzx7PrValue::Lfo::AmsFloat::min, Opzx7PrValue::Lfo::AmsFloat::max, Opzx7PrValue::Lfo::AmsFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::GlLfo::pmd, code + Opzx7PrName::GlLfo::pmd, Opzx7PrValue::Lfo::PmdFloat::min, Opzx7PrValue::Lfo::PmdFloat::max, Opzx7PrValue::Lfo::PmdFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::GlLfo::amd, code + Opzx7PrName::GlLfo::amd, Opzx7PrValue::Lfo::AmdFloat::min, Opzx7PrValue::Lfo::AmdFloat::max, Opzx7PrValue::Lfo::AmdFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::panpot, code + Opzx7PrName::panpot, Opzx7PrValue::Panpot::min, Opzx7PrValue::Panpot::max, Opzx7PrValue::Panpot::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Opzx7PrKey::panpot_en, code + Opzx7PrName::panpot_en, Opzx7PrValue::PanpotEnable::initial)); // PM Enable (Switch)

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String prefix = code + Opzx7PrKey::op + juce::String(op);
        juce::String namePrefix = code + Opzx7PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::mul, namePrefix + Opzx7PrName::mul, Opzx7PrValue::Op::Mul::min, Opzx7PrValue::Op::Mul::max, Opzx7PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::mulRatio, namePrefix + Opzx7PrName::mulRatio, Opzx7PrValue::Op::MulRatio::min, Opzx7PrValue::Op::MulRatio::max, Opzx7PrValue::Op::MulRatio::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::dt, namePrefix + Opzx7PrName::dt1, Opzx7PrValue::Op::Dt1::min, Opzx7PrValue::Op::Dt1::max, Opzx7PrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::dt2, namePrefix + Opzx7PrName::dt2, Opzx7PrValue::Op::Dt2::min, Opzx7PrValue::Op::Dt2::max, Opzx7PrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::tl, namePrefix + Opzx7PrName::tl, Opzx7PrValue::Op::Adsr::Tl::min, Opzx7PrValue::Op::Adsr::Tl::max, Opzx7PrValue::Op::Adsr::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::ar, namePrefix + Opzx7PrName::ar, Opzx7PrValue::Op::Adsr::Ar::min, Opzx7PrValue::Op::Adsr::Ar::max, Opzx7PrValue::Op::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::d1r, namePrefix + Opzx7PrName::d1r, Opzx7PrValue::Op::Adsr::D1r::min, Opzx7PrValue::Op::Adsr::D1r::max, Opzx7PrValue::Op::Adsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::d1l, namePrefix + Opzx7PrName::d1l, Opzx7PrValue::Op::Adsr::D1l::min, Opzx7PrValue::Op::Adsr::D1l::max, Opzx7PrValue::Op::Adsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::d2r, namePrefix + Opzx7PrName::d2r, Opzx7PrValue::Op::Adsr::D2r::min, Opzx7PrValue::Op::Adsr::D2r::max, Opzx7PrValue::Op::Adsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::rr, namePrefix + Opzx7PrName::rr, Opzx7PrValue::Op::Adsr::Rr::min, Opzx7PrValue::Op::Adsr::Rr::max, Opzx7PrValue::Op::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::se, namePrefix + Opzx7PrName::se, Opzx7PrValue::Op::Se::min, Opzx7PrValue::Op::Se::max, Opzx7PrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::seFreq, namePrefix + Opzx7PrName::seFreq, Opzx7PrValue::Op::SeFreq::min, Opzx7PrValue::Op::SeFreq::max, Opzx7PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::ks, namePrefix + Opzx7PrName::ks, Opzx7PrValue::Op::Ks::min, Opzx7PrValue::Op::Ks::max, Opzx7PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::phaseOffset, namePrefix + Opzx7PrName::phaseOffset, Opzx7PrValue::Op::PhaseOffset::min, Opzx7PrValue::Op::PhaseOffset::max, Opzx7PrValue::Op::PhaseOffset::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::PitchAdsr::enable, namePrefix + Opzx7PrName::PitchAdsr::enable, Opzx7PrValue::Op::PitchAdsr::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::fix, namePrefix + Opzx7PrName::fix, Opzx7PrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::fixFreq, namePrefix + Opzx7PrName::fixFreq, Opzx7PrValue::Op::FixFreq::min, Opzx7PrValue::Op::FixFreq::max, Opzx7PrValue::Op::FixFreq::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::ws, namePrefix + Opzx7PrName::ws, Opzx7PrValue::Op::Ws::min, Opzx7PrValue::Op::Ws::max, Opzx7PrValue::Op::Ws::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::pcmOffset, namePrefix + Opzx7PrName::pcmOffset, Opzx7PrValue::Op::Offset::min, Opzx7PrValue::Op::Offset::max, Opzx7PrValue::Op::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::pcmRatio, namePrefix + Opzx7PrName::pcmRatio, Opzx7PrValue::Op::Ratio::min, Opzx7PrValue::Op::Ratio::max, Opzx7PrValue::Op::Ratio::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::lfoFreq, namePrefix + Opzx7PrName::lfoFreq, Opzx7PrValue::Lfo::Freq::min, Opzx7PrValue::Lfo::Freq::max, Opzx7PrValue::Lfo::Freq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::syncDelay, namePrefix + Opzx7PrName::syncDelay, Opzx7PrValue::Op::Lfo::SyncDelay::min, Opzx7PrValue::Op::Lfo::SyncDelay::max, Opzx7PrValue::Op::Lfo::SyncDelay::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::am, namePrefix + Opzx7PrName::am, Opzx7PrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::vib, namePrefix + Opzx7PrName::vib, Opzx7PrValue::Op::Lfo::Vib::initial)); // PM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::pgShape, namePrefix + Opzx7PrName::pgShape, Opzx7PrValue::Op::Lfo::Shape::min, Opzx7PrValue::Op::Lfo::Shape::max, Opzx7PrValue::Op::Lfo::Shape::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::egShape, namePrefix + Opzx7PrName::egShape, Opzx7PrValue::Op::Lfo::Shape::min, Opzx7PrValue::Op::Lfo::Shape::max, Opzx7PrValue::Op::Lfo::Shape::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::pms, namePrefix + Opzx7PrName::pms, Opzx7PrValue::Op::Lfo::PmsFloat::min, Opzx7PrValue::Op::Lfo::PmsFloat::max, Opzx7PrValue::Op::Lfo::PmsFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::ams, namePrefix + Opzx7PrName::ams, Opzx7PrValue::Op::Lfo::AmsFloat::min, Opzx7PrValue::Op::Lfo::AmsFloat::max, Opzx7PrValue::Op::Lfo::AmsFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::pmd, namePrefix + Opzx7PrName::pmd, Opzx7PrValue::Op::Lfo::PmdFloat::min, Opzx7PrValue::Op::Lfo::PmdFloat::max, Opzx7PrValue::Op::Lfo::PmdFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::amd, namePrefix + Opzx7PrName::amd, Opzx7PrValue::Op::Lfo::AmdFloat::min, Opzx7PrValue::Op::Lfo::AmdFloat::max, Opzx7PrValue::Op::Lfo::AmdFloat::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::mask, namePrefix + Opzx7PrName::mask, Opzx7PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::rgEn, namePrefix + Opzx7PrName::rgEn, Opzx7PrValue::Op::RgAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgAr, namePrefix + Opzx7PrName::rgAr, Opzx7PrValue::Op::RgAdsr::Ar::min, Opzx7PrValue::Op::RgAdsr::Ar::max, Opzx7PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgD1r, namePrefix + Opzx7PrName::rgD1r, Opzx7PrValue::Op::RgAdsr::D1r::min, Opzx7PrValue::Op::RgAdsr::D1r::max, Opzx7PrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgD1l, namePrefix + Opzx7PrName::rgD1l, Opzx7PrValue::Op::RgAdsr::D1l::min, Opzx7PrValue::Op::RgAdsr::D1l::max, Opzx7PrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgD2r, namePrefix + Opzx7PrName::rgD2r, Opzx7PrValue::Op::RgAdsr::D2r::min, Opzx7PrValue::Op::RgAdsr::D2r::max, Opzx7PrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgRr, namePrefix + Opzx7PrName::rgRr, Opzx7PrValue::Op::RgAdsr::Rr::min, Opzx7PrValue::Op::RgAdsr::Rr::max, Opzx7PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgTl, namePrefix + Opzx7PrName::rgTl, Opzx7PrValue::Op::RgAdsr::Tl::min, Opzx7PrValue::Op::RgAdsr::Tl::max, Opzx7PrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::sus, namePrefix + Opzx7PrName::sus, Opzx7PrValue::Op::Sus::initial)); // Sus (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::xof, namePrefix + Opzx7PrName::xof, Opzx7PrValue::Op::Xof::initial)); // Xof (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
    }
}

void Opzx7Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = Opzx7PrKey::prefix;

    params.opzx7.algorithm = (int)*apvts.getRawParameterValue(code + Opzx7PrKey::alg);

    params.opzx7.feedback = *apvts.getRawParameterValue(code + Opzx7PrKey::fb0);
    params.opzx7.feedback2 = *apvts.getRawParameterValue(code + Opzx7PrKey::fb2);

    params.opzx7.fmBitDepth = (int)*apvts.getRawParameterValue(code + Opzx7PrKey::bit);
    params.opzx7.fmRateIndex = (int)*apvts.getRawParameterValue(code + Opzx7PrKey::rate);

    params.opzx7.lfoFreq = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::freq);
    params.opzx7.pgLfoWave = (int)*apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::pgShape);
    params.opzx7.egLfoWave = (int)*apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::egShape);
    params.opzx7.lfoAmSmRt = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::amSmoothRatio);
    params.opzx7.amEnable = (*apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::am) > Opzx7PrValue::boolThread);
    params.opzx7.pmEnable = (*apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::pm) > Opzx7PrValue::boolThread);
    params.opzx7.lfoPms = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::pms);
    params.opzx7.lfoAms = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::ams);
    params.opzx7.lfoPmd = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::pmd);
    params.opzx7.lfoAmd = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::amd);
    params.opzx7.lfoSyncDelay = *apvts.getRawParameterValue(code + Opzx7PrKey::GlLfo::syncDelay);
    params.opzx7.panpot = (int)*apvts.getRawParameterValue(code + Opzx7PrKey::panpot);
    params.opzx7.panpot_enable = (*apvts.getRawParameterValue(code + Opzx7PrKey::panpot_en) > Opzx7PrValue::boolThread);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String p = code + Opzx7PrKey::op + juce::String(op);

        params.opzx7.op[op].multiple = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::mul);
        params.opzx7.op[op].mutipleRatio = *apvts.getRawParameterValue(p + Opzx7PrKey::mulRatio);
        params.opzx7.op[op].detune = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::dt);
        params.opzx7.op[op].detune2 = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::dt2);
        params.opzx7.op[op].attack = *apvts.getRawParameterValue(p + Opzx7PrKey::ar);
        params.opzx7.op[op].decay = *apvts.getRawParameterValue(p + Opzx7PrKey::d1r);
        params.opzx7.op[op].sustain = *apvts.getRawParameterValue(p + Opzx7PrKey::d1l);
        params.opzx7.op[op].release = *apvts.getRawParameterValue(p + Opzx7PrKey::rr);
        params.opzx7.op[op].keyScale = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::ks);
        params.opzx7.op[op].keyScaleLevel = 0;
        params.opzx7.op[op].totalLevel = *apvts.getRawParameterValue(p + Opzx7PrKey::tl);
        params.opzx7.op[op].sustainRate = *apvts.getRawParameterValue(p + Opzx7PrKey::d2r);

        // ラジアンに変換して組み込み(180度=π)
        float phaseDegree = *apvts.getRawParameterValue(p + Opzx7PrKey::phaseOffset);
        params.opzx7.op[op].phaseOffset = phaseDegree * juce::MathConstants<float>::pi / 180.0f;

        params.opzx7.op[op].ssgEg = 0;
        params.opzx7.op[op].ssgEg = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::se);
        params.opzx7.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + Opzx7PrKey::seFreq);

        params.opzx7.op[op].fixedMode = (*apvts.getRawParameterValue(p + Opzx7PrKey::fix) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].fixedFreq = *apvts.getRawParameterValue(p + Opzx7PrKey::fixFreq);

        params.opzx7.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::ws);
        params.opzx7.op[op].pcmOffset = *apvts.getRawParameterValue(p + Opzx7PrKey::pcmOffset);
        params.opzx7.op[op].pcmRatio = *apvts.getRawParameterValue(p + Opzx7PrKey::pcmRatio);

        params.opzx7.op[op].vibEnable = (*apvts.getRawParameterValue(p + Opzx7PrKey::vib) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].amEnable = (*apvts.getRawParameterValue(p + Opzx7PrKey::am) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].lfoFreq = *apvts.getRawParameterValue(p + Opzx7PrKey::lfoFreq);
        params.opzx7.op[op].pgLfoWave = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::pgShape);
        params.opzx7.op[op].egLfoWave = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::egShape);
        params.opzx7.op[op].pms = *apvts.getRawParameterValue(p + Opzx7PrKey::pms);
        params.opzx7.op[op].ams = *apvts.getRawParameterValue(p + Opzx7PrKey::ams);
        params.opzx7.op[op].pmd = *apvts.getRawParameterValue(p + Opzx7PrKey::pmd);
        params.opzx7.op[op].amd = *apvts.getRawParameterValue(p + Opzx7PrKey::amd);

        params.opzx7.op[op].lfoSyncDelay = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::syncDelay);
        params.opzx7.op[op].oplAms = 0.0f;
        params.opzx7.op[op].oplAmd = 0.0f;
        params.opzx7.op[op].oplPms = 0.0f;
        params.opzx7.op[op].oplPmd = 0.0f;

        params.opzx7.op[op].mask = (*apvts.getRawParameterValue(p + Opzx7PrKey::mask) > Opzx7PrValue::boolThread);

        params.opzx7.op[op].isOplMode = false;
        params.opzx7.op[op].egType = false;

        params.opzx7.op[op].regEnable = (*apvts.getRawParameterValue(p + Opzx7PrKey::rgEn) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].rar = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::rgAr);
        params.opzx7.op[op].rdr = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::rgD1r);
        params.opzx7.op[op].rsl = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::rgD1l);
        params.opzx7.op[op].rsr = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::rgD2r);
        params.opzx7.op[op].rrr = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::rgRr);
        params.opzx7.op[op].rtl = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::rgTl);

		params.opzx7.op[op].pitchEnvEnable = (*apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::enable) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].pitchAdsr.bypass = false;
        params.opzx7.op[op].pitchAdsr.ar = *apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::ar);
        params.opzx7.op[op].pitchAdsr.dr = *apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::dr);
        params.opzx7.op[op].pitchAdsr.rr = *apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::rr);
        params.opzx7.op[op].pitchAdsr.stl = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::stl);
        params.opzx7.op[op].pitchAdsr.atl = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::atl);
        params.opzx7.op[op].pitchAdsr.ssl = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::ssl);
        params.opzx7.op[op].pitchAdsr.rll = (int)*apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::rll);

        params.opzx7.op[op].susEnable = (*apvts.getRawParameterValue(p + Opzx7PrKey::sus) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].xofEnable = (*apvts.getRawParameterValue(p + Opzx7PrKey::xof) > Opzx7PrValue::boolThread);
    }
}
