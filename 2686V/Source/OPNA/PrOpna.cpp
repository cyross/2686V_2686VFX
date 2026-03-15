#include "PrOpna.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void OpnaProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::opna;

    // ==========================================
    // OPNA (YM2608) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::alg, code + PrName::Fm::Post::alg, PrValue::Opna::Alg::min, PrValue::Opna::Alg::max, PrValue::Opna::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb0, code + PrName::Fm::Post::fb0, PrValue::Fm::Fb0::min, PrValue::Fm::Fb0::max, PrValue::Fm::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::fb2, code + PrName::Fm::Post::fb2, PrValue::Fm::Fb2::min, PrValue::Fm::Fb2::max, PrValue::Fm::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::bit, code + PrName::Fm::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::rate, code + PrName::Fm::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::freq, code + PrName::Fm::Post::Lfo::freq, PrValue::Lfo::Freq::min, PrValue::Lfo::Freq::max, PrValue::Lfo::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::shape, code + PrName::Fm::Post::Lfo::shape, PrValue::Opna::LfoShape::min, PrValue::Opna::LfoShape::max, PrValue::Opna::LfoShape::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::amSmoothRatio, code + PrName::Fm::Post::Lfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::am, code + PrName::Fm::Post::Lfo::am, PrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Fm::Lfo::pm, code + PrName::Fm::Post::Lfo::pm, PrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pms, code + PrName::Fm::Post::Lfo::pms, PrValue::Lfo::N88Pms::min, PrValue::Lfo::N88Pms::max, PrValue::Lfo::N88Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::pmd, code + PrName::Fm::Post::Lfo::pmd, PrValue::Lfo::N88Pmd::min, PrValue::Lfo::N88Pmd::max, PrValue::Lfo::N88Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Fm::Lfo::amd, code + PrName::Fm::Post::Lfo::amd, PrValue::Lfo::N88Amd::min, PrValue::Lfo::N88Amd::max, PrValue::Lfo::N88Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Fm::Lfo::syncDelay, code + PrName::Fm::Post::Lfo::syncDelay, PrValue::Lfo::SyncDelay::min, PrValue::Lfo::SyncDelay::max, PrValue::Lfo::SyncDelay::initial));

    for (int op = 0; op < PrValue::Opna::ops; ++op)
    {
        juce::String prefix = code + PrKey::Innder::op + juce::String(op);
        juce::String namePrefix = code + PrName::Fm::Op::key + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::mul, namePrefix + PrName::Fm::Op::Post::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::dt, namePrefix + PrName::Fm::Op::Post::dt, PrValue::Op::Dt::min, PrValue::Op::Dt::max, PrValue::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::ar, namePrefix + PrName::Fm::Op::Post::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::dr, namePrefix + PrName::Fm::Op::Post::dr, PrValue::Adsr::Dr::min, PrValue::Adsr::Dr::max, PrValue::Adsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sr, namePrefix + PrName::Fm::Op::Post::sr, PrValue::Adsr::Sr::min, PrValue::Adsr::Sr::max, PrValue::Adsr::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::sl, namePrefix + PrName::Fm::Op::Post::sl, PrValue::Adsr::Sl::min, PrValue::Adsr::Sl::max, PrValue::Adsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::rr, namePrefix + PrName::Fm::Op::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::tl, namePrefix + PrName::Fm::Op::Post::tl, PrValue::Op::Tl::min, PrValue::Op::Tl::max, PrValue::Op::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ks, namePrefix + PrName::Fm::Op::Post::ks, PrValue::Op::Ks::min, PrValue::Op::Ks::max, PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::se, namePrefix + PrName::Fm::Op::Post::se, PrValue::Op::Se::min, PrValue::Op::Se::max, PrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::seFreq, namePrefix + PrName::Fm::Op::Post::seFreq, PrValue::Op::SeFreq::min, PrValue::Op::SeFreq::max, PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::freqs, namePrefix + PrName::Fm::Op::Post::freqs, PrValue::OpLfo::Freqs::min, PrValue::OpLfo::Freqs::max, PrValue::OpLfo::Freqs::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::am, namePrefix + PrName::Fm::Op::Post::am, PrValue::OpLfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::vib, namePrefix + PrName::Fm::Op::Post::vib, PrValue::OpLfo::Vib::initial)); // PM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::pms, namePrefix + PrName::Fm::Op::Post::pms, PrValue::OpLfo::Pms::min, PrValue::OpLfo::Pms::max, PrValue::OpLfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::ams, namePrefix + PrName::Fm::Op::Post::ams, PrValue::OpLfo::Ams::min, PrValue::OpLfo::Ams::max, PrValue::OpLfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::syncDelay, namePrefix + PrName::Fm::Op::Post::syncDelay, PrValue::OpLfo::SyncDelay::min, PrValue::OpLfo::SyncDelay::max, PrValue::OpLfo::SyncDelay::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::n88Ams, namePrefix + PrName::Fm::Op::Post::n88Ams, PrValue::Lfo::N88Ams::min, PrValue::Lfo::N88Ams::max, PrValue::Lfo::N88Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::fix, namePrefix + PrName::Fm::Op::Post::fix, PrValue::Fix::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Fm::Op::fixFreq, namePrefix + PrName::Fm::Op::Post::fixFreq, PrValue::Fix::Freq::min, PrValue::Fix::Freq::max, PrValue::Fix::Freq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::mask, namePrefix + PrName::Fm::Op::Post::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Fm::Op::rgEn, namePrefix + PrName::Fm::Op::Post::rgEn, PrValue::FmRg::En::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgAr, namePrefix + PrName::Fm::Op::Post::rgAr, PrValue::FmRg::Ar::min, PrValue::FmRg::Ar::max, PrValue::FmRg::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgDr, namePrefix + PrName::Fm::Op::Post::rgDr, PrValue::FmRg::Dr::min, PrValue::FmRg::Dr::max, PrValue::FmRg::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgSr, namePrefix + PrName::Fm::Op::Post::rgSr, PrValue::FmRg::Sr::min, PrValue::FmRg::Sr::max, PrValue::FmRg::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgSl, namePrefix + PrName::Fm::Op::Post::rgSl, PrValue::FmRg::Sl::min, PrValue::FmRg::Sl::max, PrValue::FmRg::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgRr, namePrefix + PrName::Fm::Op::Post::rgRr, PrValue::FmRg::Rr::min, PrValue::FmRg::Rr::max, PrValue::FmRg::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Fm::Op::rgTl, namePrefix + PrName::Fm::Op::Post::rgTl, PrValue::FmRg::Tl::min, PrValue::FmRg::Tl::max, PrValue::FmRg::Tl::initial));
    }
}

void OpnaProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::opna;

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
    params.lfoPmd = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::pmd);
    params.lfoAmd = (int)*apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::amd);
    params.lfoAms = 0; // グローバルAMSはN88-BASICには存在しないので0固定
    params.lfoSyncDelay = *apvts.getRawParameterValue(code + PrKey::Post::Fm::Lfo::syncDelay);

    for (int op = 0; op < PrValue::Opna::ops; ++op)
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
        params.fmOp[op].ssgEg = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::se);
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::seFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fix) > PrValue::boolThread);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::fixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].lfoFreqIndex = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::freqs);
        params.fmOp[op].vibEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::vib) > PrValue::boolThread);
        params.fmOp[op].pms = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::pms);
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::am) > PrValue::boolThread);
        params.fmOp[op].ams = (int)*apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::n88Ams);
        params.fmOp[op].amd = 0; // ローカルAMDは未使用なので0固定
        params.fmOp[op].egType = true;
        params.fmOp[op].oplAms = 0.0f;
        params.fmOp[op].oplAmd = 0.0f;
        params.fmOp[op].oplPms = 0.0f;
        params.fmOp[op].oplPmd = 0.0f;
        params.fmOp[op].lfoSyncDelay = *apvts.getRawParameterValue(p + PrKey::Post::Fm::Op::syncDelay);
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
