#include "./Core.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void Opzx7Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opzx7;

    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opzx7::Alg::min, PrValue::Opzx7::Alg::max, PrValue::Opzx7::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Fm::Fb0::min, PrValue::Fm::Fb0::max, PrValue::Fm::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::fb2, code + PrName::Fm::Post::fb2, PrValue::Fm::Fb2::min, PrValue::Fm::Fb2::max, PrValue::Fm::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Innder::pitchAdsr + PrKey::Post::bypass, code + PrName::PitchAdsr::Post::bypass, PrValue::PitchAdsr::Bypass::initial));

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::freq, code + PrName::Fm::Post::Lfo::freq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::syncDelay, code + PrName::Fm::Post::Lfo::syncDelay, PrValue::Lfo::SyncDelay::min, PrValue::Lfo::SyncDelay::max, PrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::amSmoothRatio, code + PrName::Fm::Post::Lfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pgShape, code + PrName::Fm::Post::Lfo::pgShape, PrValue::Opzx7::LfoShape::min, PrValue::Opzx7::LfoShape::max, PrValue::Opzx7::LfoShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::egShape, code + PrName::Fm::Post::Lfo::egShape, PrValue::Opzx7::LfoShape::min, PrValue::Opzx7::LfoShape::max, PrValue::Opzx7::LfoShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::am, code + PrName::Fm::Post::Lfo::am, PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::pm, code + PrName::Fm::Post::Lfo::pm, PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::pms, code + PrName::Fm::Post::Lfo::pms, PrValue::Lfo::PmsFloat::min, PrValue::Lfo::PmsFloat::max, PrValue::Lfo::PmsFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::ams, code + PrName::Fm::Post::Lfo::ams, PrValue::Lfo::AmsFloat::min, PrValue::Lfo::AmsFloat::max, PrValue::Lfo::AmsFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::pmd, code + PrName::Fm::Post::Lfo::pmd, PrValue::Lfo::PmdFloat::min, PrValue::Lfo::PmdFloat::max, PrValue::Lfo::PmdFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::amd, code + PrName::Fm::Post::Lfo::amd, PrValue::Lfo::AmdFloat::min, PrValue::Lfo::AmdFloat::max, PrValue::Lfo::AmdFloat::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::panpot, code + PrName::Fm::Post::panpot, PrValue::Opzx7::Panpot::min, PrValue::Opzx7::Panpot::max, PrValue::Opzx7::Panpot::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::panpot_en, code + PrName::Fm::Post::panpot_en, PrValue::Opzx7::PanpotEnable::initial)); // PM Enable (Switch)

    for (int op = 0; op < PrValue::Opzx7::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Opzx7::Op::Mul::min, PrValue::Opzx7::Op::Mul::max, PrValue::Opzx7::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt, namePrefix + PrName::Fm::Op::Post::dt1, PrValue::Opzx7::Op::Dt1::min, PrValue::Opzx7::Op::Dt1::max, PrValue::Opzx7::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt2, namePrefix + PrName::Fm::Op::Post::dt2, PrValue::Opzx7::Op::Dt2::min, PrValue::Opzx7::Op::Dt2::max, PrValue::Opzx7::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Op::Tl::min, PrValue::Op::Tl::max, PrValue::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d1r, namePrefix + PrName::Fm::Op::Post::d1r, PrValue::Adsr::D1r::min, PrValue::Adsr::D1r::max, PrValue::Adsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d1l, namePrefix + PrName::Fm::Op::Post::d1l, PrValue::Adsr::D1l::min, PrValue::Adsr::D1l::max, PrValue::Adsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::d2r, namePrefix + PrName::Fm::Op::Post::d2r, PrValue::Adsr::D2r::min, PrValue::Adsr::D2r::max, PrValue::Adsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::se, namePrefix + PrName::Fm::Op::Post::se, PrValue::Op::Se::min, PrValue::Op::Se::max, PrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::seFreq, namePrefix + PrName::Fm::Op::Post::seFreq, PrValue::Op::SeFreq::min, PrValue::Op::SeFreq::max, PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ks, namePrefix + PrName::Fm::Op::Post::ks, PrValue::Op::Ks::min, PrValue::Op::Ks::max, PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::phaseOffset, namePrefix + PrName::Fm::Op::Post::phaseOffset, PrValue::Opzx7::Op::PhaseOffset::min, PrValue::Opzx7::Op::PhaseOffset::max, PrValue::Opzx7::Op::PhaseOffset::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::PitchAdsr::enable, namePrefix + PrName::PitchAdsr::Post::enable, PrValue::PitchAdsr::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::fix, namePrefix + PrName::Fm::Op::Post::fix, PrValue::Fix::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::fixFreq, namePrefix + PrName::Fm::Op::Post::fixFreq, PrValue::Fix::Freq::min, PrValue::Fix::Freq::max, PrValue::Fix::Freq::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ws, namePrefix + PrName::Fm::Op::Post::ws, PrValue::Opzx7::Op::Ws::min, PrValue::Opzx7::Op::Ws::max, PrValue::Opzx7::Op::Ws::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pcmOffset, namePrefix + PrName::Fm::Op::Post::pcmOffset, PrValue::Pcm::Offset::min, PrValue::Pcm::Offset::max, PrValue::Pcm::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pcmRatio, namePrefix + PrName::Fm::Op::Post::pcmRatio, PrValue::Pcm::Ratio::min, PrValue::Pcm::Ratio::max, PrValue::Pcm::Ratio::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::lfoFreq, namePrefix + PrName::Fm::Op::Post::lfoFreq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::syncDelay, namePrefix + PrName::Fm::Op::Post::syncDelay, PrValue::OpLfo::SyncDelay::min, PrValue::OpLfo::SyncDelay::max, PrValue::OpLfo::SyncDelay::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::am, namePrefix + PrName::Fm::Op::Post::am, PrValue::OpLfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::vib, namePrefix + PrName::Fm::Op::Post::vib, PrValue::OpLfo::Vib::initial)); // PM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::pgShape, namePrefix + PrName::Fm::Op::Post::pgShape, PrValue::Opzx7::LfoShape::min, PrValue::Opzx7::LfoShape::max, PrValue::Opzx7::LfoShape::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::egShape, namePrefix + PrName::Fm::Op::Post::egShape, PrValue::Opzx7::LfoShape::min, PrValue::Opzx7::LfoShape::max, PrValue::Opzx7::LfoShape::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pms, namePrefix + PrName::Fm::Op::Post::pms, PrValue::Lfo::PmsFloat::min, PrValue::Lfo::PmsFloat::max, PrValue::Lfo::PmsFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ams, namePrefix + PrName::Fm::Op::Post::ams, PrValue::Lfo::AmsFloat::min, PrValue::Lfo::AmsFloat::max, PrValue::Lfo::AmsFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::pmd, namePrefix + PrName::Fm::Op::Post::pmd, PrValue::Lfo::PmdFloat::min, PrValue::Lfo::PmdFloat::max, PrValue::Lfo::PmdFloat::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::amd, namePrefix + PrName::Fm::Op::Post::amd, PrValue::Lfo::AmdFloat::min, PrValue::Lfo::AmdFloat::max, PrValue::Lfo::AmdFloat::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::rgEn, namePrefix + PrName::Fm::Op::Post::rgEn, PrValue::FmRg::En::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgAr, namePrefix + PrName::Fm::Op::Post::rgAr, PrValue::Opzx7::Op::Ar::min, PrValue::Opzx7::Op::Ar::max, PrValue::Opzx7::Op::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgD1r, namePrefix + PrName::Fm::Op::Post::rgD1r, PrValue::Opzx7::Op::D1r::min, PrValue::Opzx7::Op::D1r::max, PrValue::Opzx7::Op::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgD1l, namePrefix + PrName::Fm::Op::Post::rgD1l, PrValue::Opzx7::Op::D1l::min, PrValue::Opzx7::Op::D1l::max, PrValue::Opzx7::Op::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgD2r, namePrefix + PrName::Fm::Op::Post::rgD2r, PrValue::Opzx7::Op::D2r::min, PrValue::Opzx7::Op::D2r::max, PrValue::Opzx7::Op::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgRr, namePrefix + PrName::Fm::Op::Post::rgRr, PrValue::Opzx7::Op::Rr::min, PrValue::Opzx7::Op::Rr::max, PrValue::Opzx7::Op::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgTl, namePrefix + PrName::Fm::Op::Post::rgTl, PrValue::Opzx7::Op::Tl::min, PrValue::Opzx7::Op::Tl::max, PrValue::Opzx7::Op::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::sus, namePrefix + PrName::Fm::Op::Post::sus, PrValue::Opzx7::Op::Sus::initial)); // Sus (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::xof, namePrefix + PrName::Fm::Op::Post::xof, PrValue::Opzx7::Op::Xof::initial)); // Xof (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
    }
}

void Opzx7Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opzx7;

    params.opzx7.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::alg);

    params.opzx7.feedback = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb0);
    params.opzx7.feedback2 = *apvts.getRawParameterValue(code + PrKey::Post::Fm::fb2);

    params.opzx7.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::bit);
    params.opzx7.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::rate);

    params.opzx7.lfoFreq = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::freq);
    params.opzx7.pgLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pgShape);
    params.opzx7.egLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::egShape);
    params.opzx7.lfoAmSmRt = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amSmoothRatio);
    params.opzx7.amEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::am) > PrValue::boolThread);
    params.opzx7.pmEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pm) > PrValue::boolThread);
    params.opzx7.lfoPms = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pms);
    params.opzx7.lfoAms = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::ams);
    params.opzx7.lfoPmd = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pmd);
    params.opzx7.lfoAmd = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amd);
    params.opzx7.lfoSyncDelay = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::syncDelay);
    params.opzx7.panpot = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::panpot);
    params.opzx7.panpot_enable = (*apvts.getRawParameterValue(code + PrKey::Post::Fm::panpot_en) > PrValue::boolThread);

    for (int op = 0; op < PrValue::Opzx7::ops; ++op)
    {
        juce::String p = code + PrKey::Innder::op + juce::String(op);

        params.opzx7.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mul);
        params.opzx7.op[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt);
        params.opzx7.op[op].detune2 = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::dt2);
        params.opzx7.op[op].attack = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ar);
        params.opzx7.op[op].decay = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::d1r);
        params.opzx7.op[op].sustain = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::d1l);
        params.opzx7.op[op].release = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rr);
        params.opzx7.op[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ks);
        params.opzx7.op[op].keyScaleLevel = 0;
        params.opzx7.op[op].totalLevel = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::tl);
        params.opzx7.op[op].sustainRate = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::d2r);

        // ラジアンに変換して組み込み(180度=π)
        float phaseDegree = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::phaseOffset);
        params.opzx7.op[op].phaseOffset = phaseDegree * juce::MathConstants<float>::pi / 180.0f;

        params.opzx7.op[op].ssgEg = 0;
        params.opzx7.op[op].ssgEg = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::se);
        params.opzx7.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::seFreq);

        params.opzx7.op[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fix) > PrValue::boolThread);
        params.opzx7.op[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fixFreq);

        params.opzx7.op[op].waveSelect = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ws);
        params.opzx7.op[op].pcmOffset = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pcmOffset);
        params.opzx7.op[op].pcmRatio = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pcmRatio);

        params.opzx7.op[op].vibEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::vib) > PrValue::boolThread);
        params.opzx7.op[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::am) > PrValue::boolThread);
        params.opzx7.op[op].lfoFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::lfoFreq);
        params.opzx7.op[op].pgLfoWave = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pgShape);
        params.opzx7.op[op].egLfoWave = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::egShape);
        params.opzx7.op[op].pms = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pms);
        params.opzx7.op[op].ams = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::ams);
        params.opzx7.op[op].pmd = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pmd);
        params.opzx7.op[op].amd = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::amd);

        params.opzx7.op[op].lfoSyncDelay = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::syncDelay);
        params.opzx7.op[op].oplAms = 0.0f;
        params.opzx7.op[op].oplAmd = 0.0f;
        params.opzx7.op[op].oplPms = 0.0f;
        params.opzx7.op[op].oplPmd = 0.0f;

        params.opzx7.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::mask) > PrValue::boolThread);

        params.opzx7.op[op].isOplMode = false;
        params.opzx7.op[op].egType = false;

        params.opzx7.op[op].regEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgEn) > PrValue::boolThread);
        params.opzx7.op[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgAr);
        params.opzx7.op[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD1r);
        params.opzx7.op[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD1l);
        params.opzx7.op[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgD2r);
        params.opzx7.op[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgRr);
        params.opzx7.op[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::rgTl);

		params.opzx7.op[op].pitchEnvEnable = (*apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::enable) > PrValue::boolThread);
        params.opzx7.op[op].pitchAdsr.bypass = false;
        params.opzx7.op[op].pitchAdsr.ar = *apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::ar);
        params.opzx7.op[op].pitchAdsr.dr = *apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::dr);
        params.opzx7.op[op].pitchAdsr.rr = *apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::rr);
        params.opzx7.op[op].pitchAdsr.stl = (int)*apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::stl);
        params.opzx7.op[op].pitchAdsr.atl = (int)*apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::atl);
        params.opzx7.op[op].pitchAdsr.ssl = (int)*apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::ssl);
        params.opzx7.op[op].pitchAdsr.rll = (int)*apvts.getRawParameterValue(p + PrKey::Post::PitchAdsr::rll);

        params.opzx7.op[op].susEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::sus) > PrValue::boolThread);
        params.opzx7.op[op].xofEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::xof) > PrValue::boolThread);
    }
}
