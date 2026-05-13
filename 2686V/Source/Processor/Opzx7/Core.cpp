#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void Opzx7Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::alg, code + PrName::alg, PrValue::Alg::min, PrValue::Alg::max, PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb0, code + PrName::fb0, PrValue::Fb0::min, PrValue::Fb0::max, PrValue::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb2, code + PrName::fb2, PrValue::Fb2::min, PrValue::Fb2::max, PrValue::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::bit, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial)); // Default 6 (16kHz)

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::GlLfo::freq, code + PrName::GlLfo::freq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::GlLfo::syncDelay, code + PrName::GlLfo::syncDelay, PrValue::Lfo::SyncDelay::min, PrValue::Lfo::SyncDelay::max, PrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::GlLfo::amSmoothRatio, code + PrName::GlLfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::GlLfo::pgShape, code + PrName::GlLfo::pgShape, PrValue::Lfo::Shape::min, PrValue::Lfo::Shape::max, PrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::GlLfo::egShape, code + PrName::GlLfo::egShape, PrValue::Lfo::Shape::min, PrValue::Lfo::Shape::max, PrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::GlLfo::am, code + PrName::GlLfo::am, PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::GlLfo::pm, code + PrName::GlLfo::pm, PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::GlLfo::pms, code + PrName::GlLfo::pms, PrValue::Lfo::PmsFloat::min, PrValue::Lfo::PmsFloat::max, PrValue::Lfo::PmsFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::GlLfo::ams, code + PrName::GlLfo::ams, PrValue::Lfo::AmsFloat::min, PrValue::Lfo::AmsFloat::max, PrValue::Lfo::AmsFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::GlLfo::pmd, code + PrName::GlLfo::pmd, PrValue::Lfo::PmdFloat::min, PrValue::Lfo::PmdFloat::max, PrValue::Lfo::PmdFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::GlLfo::amd, code + PrName::GlLfo::amd, PrValue::Lfo::AmdFloat::min, PrValue::Lfo::AmdFloat::max, PrValue::Lfo::AmdFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::panpot, code + PrName::panpot, PrValue::Panpot::min, PrValue::Panpot::max, PrValue::Panpot::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::panpot_en, code + PrName::panpot_en, PrValue::PanpotEnable::initial)); // PM Enable (Switch)

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String prefix = code + PrKey::op + juce::String(op);
        juce::String namePrefix = code + PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::mul, namePrefix + PrName::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::dt, namePrefix + PrName::dt1, PrValue::Op::Dt1::min, PrValue::Op::Dt1::max, PrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::dt2, namePrefix + PrName::dt2, PrValue::Op::Dt2::min, PrValue::Op::Dt2::max, PrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::tl, namePrefix + PrName::tl, PrValue::Op::Adsr::Tl::min, PrValue::Op::Adsr::Tl::max, PrValue::Op::Adsr::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::ar, namePrefix + PrName::ar, PrValue::Op::Adsr::Ar::min, PrValue::Op::Adsr::Ar::max, PrValue::Op::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::d1r, namePrefix + PrName::d1r, PrValue::Op::Adsr::D1r::min, PrValue::Op::Adsr::D1r::max, PrValue::Op::Adsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::d1l, namePrefix + PrName::d1l, PrValue::Op::Adsr::D1l::min, PrValue::Op::Adsr::D1l::max, PrValue::Op::Adsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::d2r, namePrefix + PrName::d2r, PrValue::Op::Adsr::D2r::min, PrValue::Op::Adsr::D2r::max, PrValue::Op::Adsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::rr, namePrefix + PrName::rr, PrValue::Op::Adsr::Rr::min, PrValue::Op::Adsr::Rr::max, PrValue::Op::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::se, namePrefix + PrName::se, PrValue::Op::Se::min, PrValue::Op::Se::max, PrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::seFreq, namePrefix + PrName::seFreq, PrValue::Op::SeFreq::min, PrValue::Op::SeFreq::max, PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::ks, namePrefix + PrName::ks, PrValue::Op::Ks::min, PrValue::Op::Ks::max, PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::phaseOffset, namePrefix + PrName::phaseOffset, PrValue::Op::PhaseOffset::min, PrValue::Op::PhaseOffset::max, PrValue::Op::PhaseOffset::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::PitchAdsr::enable, namePrefix + PrName::PitchAdsr::enable, PrValue::Op::PitchAdsr::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::fix, namePrefix + PrName::fix, PrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::fixFreq, namePrefix + PrName::fixFreq, PrValue::Op::FixFreq::min, PrValue::Op::FixFreq::max, PrValue::Op::FixFreq::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::ws, namePrefix + PrName::ws, PrValue::Op::Ws::min, PrValue::Op::Ws::max, PrValue::Op::Ws::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::pcmOffset, namePrefix + PrName::pcmOffset, PrValue::Op::Offset::min, PrValue::Op::Offset::max, PrValue::Op::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::pcmRatio, namePrefix + PrName::pcmRatio, PrValue::Op::Ratio::min, PrValue::Op::Ratio::max, PrValue::Op::Ratio::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::lfoFreq, namePrefix + PrName::lfoFreq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::syncDelay, namePrefix + PrName::syncDelay, PrValue::Op::Lfo::SyncDelay::min, PrValue::Op::Lfo::SyncDelay::max, PrValue::Op::Lfo::SyncDelay::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::am, namePrefix + PrName::am, PrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::vib, namePrefix + PrName::vib, PrValue::Op::Lfo::Vib::initial)); // PM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::pgShape, namePrefix + PrName::pgShape, PrValue::Op::Lfo::Shape::min, PrValue::Op::Lfo::Shape::max, PrValue::Op::Lfo::Shape::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::egShape, namePrefix + PrName::egShape, PrValue::Op::Lfo::Shape::min, PrValue::Op::Lfo::Shape::max, PrValue::Op::Lfo::Shape::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::pms, namePrefix + PrName::pms, PrValue::Op::Lfo::PmsFloat::min, PrValue::Op::Lfo::PmsFloat::max, PrValue::Op::Lfo::PmsFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::ams, namePrefix + PrName::ams, PrValue::Op::Lfo::AmsFloat::min, PrValue::Op::Lfo::AmsFloat::max, PrValue::Op::Lfo::AmsFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::pmd, namePrefix + PrName::pmd, PrValue::Op::Lfo::PmdFloat::min, PrValue::Op::Lfo::PmdFloat::max, PrValue::Op::Lfo::PmdFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::amd, namePrefix + PrName::amd, PrValue::Op::Lfo::AmdFloat::min, PrValue::Op::Lfo::AmdFloat::max, PrValue::Op::Lfo::AmdFloat::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::mask, namePrefix + PrName::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::rgEn, namePrefix + PrName::rgEn, PrValue::Op::RgAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgAr, namePrefix + PrName::rgAr, PrValue::Op::RgAdsr::Ar::min, PrValue::Op::RgAdsr::Ar::max, PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgD1r, namePrefix + PrName::rgD1r, PrValue::Op::RgAdsr::D1r::min, PrValue::Op::RgAdsr::D1r::max, PrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgD1l, namePrefix + PrName::rgD1l, PrValue::Op::RgAdsr::D1l::min, PrValue::Op::RgAdsr::D1l::max, PrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgD2r, namePrefix + PrName::rgD2r, PrValue::Op::RgAdsr::D2r::min, PrValue::Op::RgAdsr::D2r::max, PrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgRr, namePrefix + PrName::rgRr, PrValue::Op::RgAdsr::Rr::min, PrValue::Op::RgAdsr::Rr::max, PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgTl, namePrefix + PrName::rgTl, PrValue::Op::RgAdsr::Tl::min, PrValue::Op::RgAdsr::Tl::max, PrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::sus, namePrefix + PrName::sus, PrValue::Op::Sus::initial)); // Sus (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::xof, namePrefix + PrName::xof, PrValue::Op::Xof::initial)); // Xof (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
    }
}

void Opzx7Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    params.opzx7.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::alg);

    params.opzx7.feedback = *apvts.getRawParameterValue(code + PrKey::fb0);
    params.opzx7.feedback2 = *apvts.getRawParameterValue(code + PrKey::fb2);

    params.opzx7.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::bit);
    params.opzx7.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);

    params.opzx7.lfoFreq = *apvts.getRawParameterValue(code + PrKey::GlLfo::freq);
    params.opzx7.pgLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::GlLfo::pgShape);
    params.opzx7.egLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::GlLfo::egShape);
    params.opzx7.lfoAmSmRt = *apvts.getRawParameterValue(code + PrKey::GlLfo::amSmoothRatio);
    params.opzx7.amEnable = (*apvts.getRawParameterValue(code + PrKey::GlLfo::am) > PrValue::boolThread);
    params.opzx7.pmEnable = (*apvts.getRawParameterValue(code + PrKey::GlLfo::pm) > PrValue::boolThread);
    params.opzx7.lfoPms = *apvts.getRawParameterValue(code + PrKey::GlLfo::pms);
    params.opzx7.lfoAms = *apvts.getRawParameterValue(code + PrKey::GlLfo::ams);
    params.opzx7.lfoPmd = *apvts.getRawParameterValue(code + PrKey::GlLfo::pmd);
    params.opzx7.lfoAmd = *apvts.getRawParameterValue(code + PrKey::GlLfo::amd);
    params.opzx7.lfoSyncDelay = *apvts.getRawParameterValue(code + PrKey::GlLfo::syncDelay);
    params.opzx7.panpot = (int)*apvts.getRawParameterValue(code + PrKey::panpot);
    params.opzx7.panpot_enable = (*apvts.getRawParameterValue(code + PrKey::panpot_en) > PrValue::boolThread);

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String p = code + PrKey::op + juce::String(op);

        params.opzx7.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::mul);
        params.opzx7.op[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::dt);
        params.opzx7.op[op].detune2 = (int)*apvts.getRawParameterValue(p + PrKey::dt2);
        params.opzx7.op[op].attack = *apvts.getRawParameterValue(p + PrKey::ar);
        params.opzx7.op[op].decay = *apvts.getRawParameterValue(p + PrKey::d1r);
        params.opzx7.op[op].sustain = *apvts.getRawParameterValue(p + PrKey::d1l);
        params.opzx7.op[op].release = *apvts.getRawParameterValue(p + PrKey::rr);
        params.opzx7.op[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::ks);
        params.opzx7.op[op].keyScaleLevel = 0;
        params.opzx7.op[op].totalLevel = *apvts.getRawParameterValue(p + PrKey::tl);
        params.opzx7.op[op].sustainRate = *apvts.getRawParameterValue(p + PrKey::d2r);

        // ラジアンに変換して組み込み(180度=π)
        float phaseDegree = *apvts.getRawParameterValue(p + PrKey::phaseOffset);
        params.opzx7.op[op].phaseOffset = phaseDegree * juce::MathConstants<float>::pi / 180.0f;

        params.opzx7.op[op].ssgEg = 0;
        params.opzx7.op[op].ssgEg = (int)*apvts.getRawParameterValue(p + PrKey::se);
        params.opzx7.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::seFreq);

        params.opzx7.op[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::fix) > PrValue::boolThread);
        params.opzx7.op[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::fixFreq);

        params.opzx7.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + PrKey::ws);
        params.opzx7.op[op].pcmOffset = *apvts.getRawParameterValue(p + PrKey::pcmOffset);
        params.opzx7.op[op].pcmRatio = *apvts.getRawParameterValue(p + PrKey::pcmRatio);

        params.opzx7.op[op].vibEnable = (*apvts.getRawParameterValue(p + PrKey::vib) > PrValue::boolThread);
        params.opzx7.op[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::am) > PrValue::boolThread);
        params.opzx7.op[op].lfoFreq = *apvts.getRawParameterValue(p + PrKey::lfoFreq);
        params.opzx7.op[op].pgLfoWave = (int)*apvts.getRawParameterValue(p + PrKey::pgShape);
        params.opzx7.op[op].egLfoWave = (int)*apvts.getRawParameterValue(p + PrKey::egShape);
        params.opzx7.op[op].pms = *apvts.getRawParameterValue(p + PrKey::pms);
        params.opzx7.op[op].ams = *apvts.getRawParameterValue(p + PrKey::ams);
        params.opzx7.op[op].pmd = *apvts.getRawParameterValue(p + PrKey::pmd);
        params.opzx7.op[op].amd = *apvts.getRawParameterValue(p + PrKey::amd);

        params.opzx7.op[op].lfoSyncDelay = (int)*apvts.getRawParameterValue(p + PrKey::syncDelay);
        params.opzx7.op[op].oplAms = 0.0f;
        params.opzx7.op[op].oplAmd = 0.0f;
        params.opzx7.op[op].oplPms = 0.0f;
        params.opzx7.op[op].oplPmd = 0.0f;

        params.opzx7.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::mask) > PrValue::boolThread);

        params.opzx7.op[op].isOplMode = false;
        params.opzx7.op[op].egType = false;

        params.opzx7.op[op].regEnable = (*apvts.getRawParameterValue(p + PrKey::rgEn) > PrValue::boolThread);
        params.opzx7.op[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::rgAr);
        params.opzx7.op[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::rgD1r);
        params.opzx7.op[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::rgD1l);
        params.opzx7.op[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::rgD2r);
        params.opzx7.op[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::rgRr);
        params.opzx7.op[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::rgTl);

		params.opzx7.op[op].pitchEnvEnable = (*apvts.getRawParameterValue(p + PrKey::PitchAdsr::enable) > PrValue::boolThread);
        params.opzx7.op[op].pitchAdsr.bypass = false;
        params.opzx7.op[op].pitchAdsr.ar = *apvts.getRawParameterValue(p + PrKey::PitchAdsr::ar);
        params.opzx7.op[op].pitchAdsr.dr = *apvts.getRawParameterValue(p + PrKey::PitchAdsr::dr);
        params.opzx7.op[op].pitchAdsr.rr = *apvts.getRawParameterValue(p + PrKey::PitchAdsr::rr);
        params.opzx7.op[op].pitchAdsr.stl = (int)*apvts.getRawParameterValue(p + PrKey::PitchAdsr::stl);
        params.opzx7.op[op].pitchAdsr.atl = (int)*apvts.getRawParameterValue(p + PrKey::PitchAdsr::atl);
        params.opzx7.op[op].pitchAdsr.ssl = (int)*apvts.getRawParameterValue(p + PrKey::PitchAdsr::ssl);
        params.opzx7.op[op].pitchAdsr.rll = (int)*apvts.getRawParameterValue(p + PrKey::PitchAdsr::rll);

        params.opzx7.op[op].susEnable = (*apvts.getRawParameterValue(p + PrKey::sus) > PrValue::boolThread);
        params.opzx7.op[op].xofEnable = (*apvts.getRawParameterValue(p + PrKey::xof) > PrValue::boolThread);
    }
}
