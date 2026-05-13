
#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OpmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::alg, code + PrName::alg, PrValue::Alg::min, PrValue::Alg::max, PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb0, code + PrName::fb0, PrValue::Fb0::min, PrValue::Fb0::max, PrValue::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb2, code + PrName::fb2, PrValue::Fb2::min, PrValue::Fb2::max, PrValue::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::bit, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::freq, code + PrName::Lfo::freq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Lfo::amSmoothRatio, code + PrName::Lfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::pgShape, code + PrName::Lfo::pgShape, PrValue::Lfo::PgShape::min, PrValue::Lfo::PgShape::max, PrValue::Lfo::PgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::egShape, code + PrName::Lfo::egShape, PrValue::Lfo::EgShape::min, PrValue::Lfo::EgShape::max, PrValue::Lfo::EgShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Lfo::am, code + PrName::Lfo::am, PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Lfo::pm, code + PrName::Lfo::pm, PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::pms, code + PrName::Lfo::pms, PrValue::Lfo::Pms::min, PrValue::Lfo::Pms::max, PrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::ams, code + PrName::Lfo::ams, PrValue::Lfo::Ams::min, PrValue::Lfo::Ams::max, PrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::pmd, code + PrName::Lfo::pmd, PrValue::Lfo::Pmd::min, PrValue::Lfo::Pmd::max, PrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::amd, code + PrName::Lfo::amd, PrValue::Lfo::Amd::min, PrValue::Lfo::Amd::max, PrValue::Lfo::Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Lfo::syncDelay, code + PrName::Lfo::syncDelay, PrValue::Lfo::SyncDelayOpm::min, PrValue::Lfo::SyncDelayOpm::max, PrValue::Lfo::SyncDelayOpm::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::pan, code + PrName::pan, PrValue::Pan::min, PrValue::Pan::max, PrValue::Pan::initial));

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String prefix = code + PrKey::op + juce::String(op);
        juce::String namePrefix = code + PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::mul, namePrefix + PrName::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::dt, namePrefix + PrName::dt1, PrValue::Op::Dt1::min, PrValue::Op::Dt1::max, PrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::dt2, namePrefix + PrName::dt2, PrValue::Op::Dt2::min, PrValue::Op::Dt2::max, PrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::seFreq, namePrefix + PrName::seFreq, PrValue::Op::SeFreq::min, PrValue::Op::SeFreq::max, PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::ks, namePrefix + PrName::ks, PrValue::Op::Ks::min, PrValue::Op::Ks::max, PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::amsEn, namePrefix + PrName::amsEn, PrValue::Op::Lfo::AmsEn::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::fix, namePrefix + PrName::fix, PrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::fixFreq, namePrefix + PrName::fixFreq, PrValue::Op::FixFreq::min, PrValue::Op::FixFreq::max, PrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::mask, namePrefix + PrName::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgAr, namePrefix + PrName::rgAr, PrValue::Op::RgAdsr::Ar::min, PrValue::Op::RgAdsr::Ar::max, PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgD1r, namePrefix + PrName::rgD1r, PrValue::Op::RgAdsr::D1r::min, PrValue::Op::RgAdsr::D1r::max, PrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgD1l, namePrefix + PrName::rgD1l, PrValue::Op::RgAdsr::D1l::min, PrValue::Op::RgAdsr::D1l::max, PrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgD2r, namePrefix + PrName::rgD2r, PrValue::Op::RgAdsr::D2r::min, PrValue::Op::RgAdsr::D2r::max, PrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgRr, namePrefix + PrName::rgRr, PrValue::Op::RgAdsr::Rr::min, PrValue::Op::RgAdsr::Rr::max, PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgTl, namePrefix + PrName::rgTl, PrValue::Op::RgAdsr::Tl::min, PrValue::Op::RgAdsr::Tl::max, PrValue::Op::RgAdsr::Tl::initial));
    }
}

void OpmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    params.opm.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::alg);
    params.opm.feedback = *apvts.getRawParameterValue(code + PrKey::fb0);
    params.opm.feedback2 = *apvts.getRawParameterValue(code + PrKey::fb2);
    params.opm.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::bit);
    params.opm.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);
    params.opm.lfoFreq = *apvts.getRawParameterValue(code + PrKey::Lfo::freq);
    params.opm.lfoWave = 0;
    params.opm.pgLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::pgShape);
    params.opm.egLfoWave = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::egShape);
    params.opm.lfoAmSmRt = *apvts.getRawParameterValue(code + PrKey::Lfo::amSmoothRatio);
    params.opm.amEnable = (*apvts.getRawParameterValue(code + PrKey::Lfo::am) > PrValue::boolThread);
    params.opm.pmEnable = (*apvts.getRawParameterValue(code + PrKey::Lfo::pm) > PrValue::boolThread);
    params.opm.lfoPms = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::pms);
    params.opm.lfoAms = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::ams);
    params.opm.lfoPmd = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::pmd);
    params.opm.lfoAmd = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::amd);
    params.opm.lfoSyncDelay = (int)*apvts.getRawParameterValue(code + PrKey::Lfo::syncDelay);
    params.opm.pan = (int)*apvts.getRawParameterValue(code + PrKey::pan);

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String p = code + PrKey::op + juce::String(op);

        params.opm.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::mul);
        params.opm.op[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::dt);
        params.opm.op[op].detune2 = (int)*apvts.getRawParameterValue(p + PrKey::dt2);
        params.opm.op[op].attack = 0.0f;
        params.opm.op[op].decay = 0.0f;
        params.opm.op[op].sustain = 0.0f;
        params.opm.op[op].release = 0.0f;
        params.opm.op[op].sustainRate = 0.0f;
        params.opm.op[op].totalLevel = 0.0f;
        params.opm.op[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::ks);
        params.opm.op[op].keyScaleLevel = 0;
        params.opm.op[op].totalLevel = 0.0f;
        params.opm.op[op].phaseOffset = 0.0f;
        params.opm.op[op].ssgEg = 0;
        params.opm.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::seFreq);
        params.opm.op[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::fix) > PrValue::boolThread);
        params.opm.op[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::fixFreq);
        params.opm.op[op].waveSelect = 0; // Sine
        params.opm.op[op].egType = true;
        params.opm.op[op].vibEnable = false;
        params.opm.op[op].pms = 0;
		params.opm.op[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::amsEn) > PrValue::boolThread); // AMS Enable を AM Enable で代用
        params.opm.op[op].ams = 0;
        params.opm.op[op].oplAms = 0.0f;
        params.opm.op[op].oplAmd = 0.0f;
        params.opm.op[op].oplPms = 0.0f;
        params.opm.op[op].oplPmd = 0.0f;
        params.opm.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::mask) > PrValue::boolThread);

        params.opm.op[op].isOplMode = false;
        params.opm.op[op].regEnable = true;
        params.opm.op[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::rgAr);
        params.opm.op[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::rgD1r);
        params.opm.op[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::rgD1l);
        params.opm.op[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::rgD2r);
        params.opm.op[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::rgRr);
        params.opm.op[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::rgTl);
    }
}
