
#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = OpmPrKey::prefix;

    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::alg, code + OpmPrName::alg, OpmPrValue::Alg::min, OpmPrValue::Alg::max, OpmPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::fb, code + OpmPrName::fb, OpmPrValue::Fb::min, OpmPrValue::Fb::max, OpmPrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::bit, code + OpmPrName::bit, OpmPrValue::Bit::min, OpmPrValue::Bit::max, OpmPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::rate, code + OpmPrName::rate, OpmPrValue::Rate::min, OpmPrValue::Rate::max, OpmPrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::freq, code + OpmPrName::Lfo::freq, OpmPrValue::Lfo::Freq::min, OpmPrValue::Lfo::Freq::max, OpmPrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + OpmPrKey::Lfo::amSmoothRatio, code + OpmPrName::Lfo::amSmoothRatio, OpmPrValue::Lfo::AmSmRt::min, OpmPrValue::Lfo::AmSmRt::max, OpmPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::pgShape, code + OpmPrName::Lfo::pgShape, OpmPrValue::Lfo::PgShape::min, OpmPrValue::Lfo::PgShape::max, OpmPrValue::Lfo::PgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::egShape, code + OpmPrName::Lfo::egShape, OpmPrValue::Lfo::EgShape::min, OpmPrValue::Lfo::EgShape::max, OpmPrValue::Lfo::EgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + OpmPrKey::Lfo::am, code + OpmPrName::Lfo::am, OpmPrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + OpmPrKey::Lfo::pm, code + OpmPrName::Lfo::pm, OpmPrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::pms, code + OpmPrName::Lfo::pms, OpmPrValue::Lfo::Pms::min, OpmPrValue::Lfo::Pms::max, OpmPrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::ams, code + OpmPrName::Lfo::ams, OpmPrValue::Lfo::Ams::min, OpmPrValue::Lfo::Ams::max, OpmPrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::pmd, code + OpmPrName::Lfo::pmd, OpmPrValue::Lfo::Pmd::min, OpmPrValue::Lfo::Pmd::max, OpmPrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::amd, code + OpmPrName::Lfo::amd, OpmPrValue::Lfo::Amd::min, OpmPrValue::Lfo::Amd::max, OpmPrValue::Lfo::Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::Lfo::syncDelay, code + OpmPrName::Lfo::syncDelay, OpmPrValue::Lfo::SyncDelayOpm::min, OpmPrValue::Lfo::SyncDelayOpm::max, OpmPrValue::Lfo::SyncDelayOpm::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpmPrKey::pan, code + OpmPrName::pan, OpmPrValue::Pan::min, OpmPrValue::Pan::max, OpmPrValue::Pan::initial));

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String prefix = code + OpmPrKey::op + juce::String(op);
        juce::String namePrefix = code + OpmPrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::mul, namePrefix + OpmPrName::mul, OpmPrValue::Op::Mul::min, OpmPrValue::Op::Mul::max, OpmPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::dt, namePrefix + OpmPrName::dt1, OpmPrValue::Op::Dt1::min, OpmPrValue::Op::Dt1::max, OpmPrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::dt2, namePrefix + OpmPrName::dt2, OpmPrValue::Op::Dt2::min, OpmPrValue::Op::Dt2::max, OpmPrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpmPrKey::seFreq, namePrefix + OpmPrName::seFreq, OpmPrValue::Op::SeFreq::min, OpmPrValue::Op::SeFreq::max, OpmPrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::ks, namePrefix + OpmPrName::ks, OpmPrValue::Op::Ks::min, OpmPrValue::Op::Ks::max, OpmPrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpmPrKey::amsEn, namePrefix + OpmPrName::amsEn, OpmPrValue::Op::Lfo::AmsEn::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpmPrKey::fix, namePrefix + OpmPrName::fix, OpmPrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpmPrKey::fixFreq, namePrefix + OpmPrName::fixFreq, OpmPrValue::Op::FixFreq::min, OpmPrValue::Op::FixFreq::max, OpmPrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpmPrKey::mask, namePrefix + OpmPrName::mask, OpmPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rgAr, namePrefix + OpmPrName::rgAr, OpmPrValue::Op::RgAdsr::Ar::min, OpmPrValue::Op::RgAdsr::Ar::max, OpmPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rgD1r, namePrefix + OpmPrName::rgD1r, OpmPrValue::Op::RgAdsr::D1r::min, OpmPrValue::Op::RgAdsr::D1r::max, OpmPrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rgD1l, namePrefix + OpmPrName::rgD1l, OpmPrValue::Op::RgAdsr::D1l::min, OpmPrValue::Op::RgAdsr::D1l::max, OpmPrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rgD2r, namePrefix + OpmPrName::rgD2r, OpmPrValue::Op::RgAdsr::D2r::min, OpmPrValue::Op::RgAdsr::D2r::max, OpmPrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rgRr, namePrefix + OpmPrName::rgRr, OpmPrValue::Op::RgAdsr::Rr::min, OpmPrValue::Op::RgAdsr::Rr::max, OpmPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpmPrKey::rgTl, namePrefix + OpmPrName::rgTl, OpmPrValue::Op::RgAdsr::Tl::min, OpmPrValue::Op::RgAdsr::Tl::max, OpmPrValue::Op::RgAdsr::Tl::initial));
    }
}

void OpmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = OpmPrKey::prefix;

    params.opm.algorithm = (int)*apvts.getRawParameterValue(code + OpmPrKey::alg);
    params.opm.feedback = *apvts.getRawParameterValue(code + OpmPrKey::fb);
    params.opm.fmBitDepth = (int)*apvts.getRawParameterValue(code + OpmPrKey::bit);
    params.opm.fmRateIndex = (int)*apvts.getRawParameterValue(code + OpmPrKey::rate);
    params.opm.lfoFreq = *apvts.getRawParameterValue(code + OpmPrKey::Lfo::freq);
    params.opm.lfoWave = 0;
    params.opm.pgLfoWave = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::pgShape);
    params.opm.egLfoWave = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::egShape);
    params.opm.lfoAmSmRt = *apvts.getRawParameterValue(code + OpmPrKey::Lfo::amSmoothRatio);
    params.opm.amEnable = (*apvts.getRawParameterValue(code + OpmPrKey::Lfo::am) > OpmPrValue::boolThread);
    params.opm.pmEnable = (*apvts.getRawParameterValue(code + OpmPrKey::Lfo::pm) > OpmPrValue::boolThread);
    params.opm.lfoPms = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::pms);
    params.opm.lfoAms = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::ams);
    params.opm.lfoPmd = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::pmd);
    params.opm.lfoAmd = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::amd);
    params.opm.lfoSyncDelay = (int)*apvts.getRawParameterValue(code + OpmPrKey::Lfo::syncDelay);
    params.opm.pan = (int)*apvts.getRawParameterValue(code + OpmPrKey::pan);

    for (int op = 0; op < OpmPrValue::ops; ++op)
    {
        juce::String p = code + OpmPrKey::op + juce::String(op);

        params.opm.op[op].multiple = (int)*apvts.getRawParameterValue(p + OpmPrKey::mul);
        params.opm.op[op].detune = (int)*apvts.getRawParameterValue(p + OpmPrKey::dt);
        params.opm.op[op].detune2 = (int)*apvts.getRawParameterValue(p + OpmPrKey::dt2);
        params.opm.op[op].attack = 0.0f;
        params.opm.op[op].decay = 0.0f;
        params.opm.op[op].sustain = 0.0f;
        params.opm.op[op].release = 0.0f;
        params.opm.op[op].sustainRate = 0.0f;
        params.opm.op[op].totalLevel = 0.0f;
        params.opm.op[op].keyScale = (int)*apvts.getRawParameterValue(p + OpmPrKey::ks);
        params.opm.op[op].keyScaleLevel = 0;
        params.opm.op[op].totalLevel = 0.0f;
        params.opm.op[op].phaseOffset = 0.0f;
        params.opm.op[op].ssgEg = 0;
        params.opm.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + OpmPrKey::seFreq);
        params.opm.op[op].fixedMode = (*apvts.getRawParameterValue(p + OpmPrKey::fix) > OpmPrValue::boolThread);
        params.opm.op[op].fixedFreq = *apvts.getRawParameterValue(p + OpmPrKey::fixFreq);
        params.opm.op[op].waveSelect = 0; // Sine
        params.opm.op[op].egType = true;
        params.opm.op[op].vibEnable = false;
        params.opm.op[op].pms = 0;
		params.opm.op[op].amEnable = (*apvts.getRawParameterValue(p + OpmPrKey::amsEn) > OpmPrValue::boolThread); // AMS Enable を AM Enable で代用
        params.opm.op[op].ams = 0;
        params.opm.op[op].oplAms = 0.0f;
        params.opm.op[op].oplAmd = 0.0f;
        params.opm.op[op].oplPms = 0.0f;
        params.opm.op[op].oplPmd = 0.0f;
        params.opm.op[op].mask = (*apvts.getRawParameterValue(p + OpmPrKey::mask) > OpmPrValue::boolThread);

        params.opm.op[op].isOplMode = false;
        params.opm.op[op].regEnable = true;
        params.opm.op[op].rar = (int)*apvts.getRawParameterValue(p + OpmPrKey::rgAr);
        params.opm.op[op].rdr = (int)*apvts.getRawParameterValue(p + OpmPrKey::rgD1r);
        params.opm.op[op].rsl = (int)*apvts.getRawParameterValue(p + OpmPrKey::rgD1l);
        params.opm.op[op].rsr = (int)*apvts.getRawParameterValue(p + OpmPrKey::rgD2r);
        params.opm.op[op].rrr = (int)*apvts.getRawParameterValue(p + OpmPrKey::rgRr);
        params.opm.op[op].rtl = (int)*apvts.getRawParameterValue(p + OpmPrKey::rgTl);
    }
}
