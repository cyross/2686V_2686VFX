#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = OpnPrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::alg, code + OpnPrName::alg, OpnPrValue::Alg::min, OpnPrValue::Alg::max, OpnPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::fb, code + OpnPrName::fb, OpnPrValue::Fb::min, OpnPrValue::Fb::max, OpnPrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::bit, code + OpnPrName::bit, OpnPrValue::Bit::min, OpnPrValue::Bit::max, OpnPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::rate, code + OpnPrName::rate, OpnPrValue::Rate::min, OpnPrValue::Rate::max, OpnPrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::N88Lfo::freq, code + OpnPrName::N88Lfo::freq, OpnPrValue::Lfo::N88Freq::min, OpnPrValue::Lfo::N88Freq::max, OpnPrValue::Lfo::N88Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::N88Lfo::shape, code + OpnPrName::N88Lfo::shape, OpnPrValue::Lfo::Shape::min, OpnPrValue::Lfo::Shape::max, OpnPrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + OpnPrKey::N88Lfo::amSmoothRatio, code + OpnPrName::N88Lfo::amSmoothRatio, OpnPrValue::Lfo::AmSmRt::min, OpnPrValue::Lfo::AmSmRt::max, OpnPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + OpnPrKey::N88Lfo::am, code + OpnPrName::N88Lfo::am, OpnPrValue::Lfo::Am::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + OpnPrKey::N88Lfo::pm, code + OpnPrName::N88Lfo::pm, OpnPrValue::Lfo::Pm::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::N88Lfo::pms, code + OpnPrName::N88Lfo::pms, OpnPrValue::Lfo::N88Pms::min, OpnPrValue::Lfo::N88Pms::max, OpnPrValue::Lfo::N88Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::N88Lfo::pmd, code + OpnPrName::N88Lfo::pmd, OpnPrValue::Lfo::N88Pmd::min, OpnPrValue::Lfo::N88Pmd::max, OpnPrValue::Lfo::N88Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::N88Lfo::amd, code + OpnPrName::N88Lfo::amd, OpnPrValue::Lfo::N88Amd::min, OpnPrValue::Lfo::N88Amd::max, OpnPrValue::Lfo::N88Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnPrKey::N88Lfo::syncDelay, code + OpnPrName::N88Lfo::syncDelay, OpnPrValue::Lfo::SyncDelay::min, OpnPrValue::Lfo::SyncDelay::max, OpnPrValue::Lfo::SyncDelay::initial));

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String prefix = code + OpnPrKey::op + juce::String(op);
        juce::String namePrefix = code + OpnPrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::mul, namePrefix + OpnPrName::mul, OpnPrValue::Op::Mul::min, OpnPrValue::Op::Mul::max, OpnPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::dt, namePrefix + OpnPrName::dt, OpnPrValue::Op::Dt::min, OpnPrValue::Op::Dt::max, OpnPrValue::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::ks, namePrefix + OpnPrName::ks, OpnPrValue::Op::Ks::min, OpnPrValue::Op::Ks::max, OpnPrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnPrKey::fix, namePrefix + OpnPrName::fix, OpnPrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpnPrKey::fixFreq, namePrefix + OpnPrName::fixFreq, OpnPrValue::Op::FixFreq::min, OpnPrValue::Op::FixFreq::max, OpnPrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::n88Ams, namePrefix + OpnPrName::n88Ams, OpnPrValue::Lfo::N88Ams::min, OpnPrValue::Lfo::N88Ams::max, OpnPrValue::Lfo::N88Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnPrKey::mask, namePrefix + OpnPrName::mask, OpnPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rgAr, namePrefix + OpnPrName::rgAr, OpnPrValue::Op::RgAdsr::Ar::min, OpnPrValue::Op::RgAdsr::Ar::max, OpnPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rgDr, namePrefix + OpnPrName::rgDr, OpnPrValue::Op::RgAdsr::Dr::min, OpnPrValue::Op::RgAdsr::Dr::max, OpnPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rgSl, namePrefix + OpnPrName::rgSl, OpnPrValue::Op::RgAdsr::Sl::min, OpnPrValue::Op::RgAdsr::Sl::max, OpnPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rgSr, namePrefix + OpnPrName::rgSr, OpnPrValue::Op::RgAdsr::Sr::min, OpnPrValue::Op::RgAdsr::Sr::max, OpnPrValue::Op::RgAdsr::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rgRr, namePrefix + OpnPrName::rgRr, OpnPrValue::Op::RgAdsr::Rr::min, OpnPrValue::Op::RgAdsr::Rr::max, OpnPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rgTl, namePrefix + OpnPrName::rgTl, OpnPrValue::Op::RgAdsr::Tl::min, OpnPrValue::Op::RgAdsr::Tl::max, OpnPrValue::Op::RgAdsr::Tl::initial));
    }
}

void OpnProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = OpnPrKey::prefix;

    params.opn.algorithm = (int)*apvts.getRawParameterValue(code + OpnPrKey::alg);
    params.opn.feedback = *apvts.getRawParameterValue(code + OpnPrKey::fb);
    params.opn.fmBitDepth = (int)*apvts.getRawParameterValue(code + OpnPrKey::bit);
    params.opn.fmRateIndex = (int)*apvts.getRawParameterValue(code + OpnPrKey::rate);
    // Int(0〜16383) として読み込み、Hzに変換する
    float n88Speed = *apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::freq);
    // ソフトウェアLFOの周波数変換公式 (60Hz割り込み、16bitカウンター)
    // f = (Speed * 60Hz) / 65536
    params.opn.lfoFreq = (n88Speed * 60.0f) / 65536.0f;
    params.opn.lfoWave = (int)*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::shape);
    params.opn.lfoAmSmRt = *apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::amSmoothRatio);
    params.opn.amEnable = (*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::am) > OpnPrValue::boolThread);
    params.opn.pmEnable = (*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::pm) > OpnPrValue::boolThread);
    params.opn.lfoPms = (int)*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::pms);
    params.opn.lfoPmd = (int)*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::pmd);
    params.opn.lfoAmd = (int)*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::amd);
    params.opn.lfoAms = 0; // グローバルAMSはN88-BASICには存在しないので0固定
    params.opn.lfoSyncDelay = (int)*apvts.getRawParameterValue(code + OpnPrKey::N88Lfo::syncDelay);

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String p = code + OpnPrKey::op + juce::String(op);

        params.opn.op[op].multiple = (int)*apvts.getRawParameterValue(p + OpnPrKey::mul);
        params.opn.op[op].detune = (int)*apvts.getRawParameterValue(p + OpnPrKey::dt);
        params.opn.op[op].fixedMode = (*apvts.getRawParameterValue(p + OpnPrKey::fix) > OpnPrValue::boolThread);
        params.opn.op[op].fixedFreq = *apvts.getRawParameterValue(p + OpnPrKey::fixFreq);
        params.opn.op[op].waveSelect = 0; // Sine
        params.opn.op[op].n88Ams = (int)*apvts.getRawParameterValue(p + OpnPrKey::n88Ams);
        params.opn.op[op].mask = (*apvts.getRawParameterValue(p + OpnPrKey::mask) > OpnPrValue::boolThread);

        params.opn.op[op].m_adsrParams.ar = (int)*apvts.getRawParameterValue(p + OpnPrKey::rgAr);
        params.opn.op[op].m_adsrParams.dr = (int)*apvts.getRawParameterValue(p + OpnPrKey::rgDr);
        params.opn.op[op].m_adsrParams.sl = (int)*apvts.getRawParameterValue(p + OpnPrKey::rgSl);
        params.opn.op[op].m_adsrParams.sr = (int)*apvts.getRawParameterValue(p + OpnPrKey::rgSr);
        params.opn.op[op].m_adsrParams.rr = (int)*apvts.getRawParameterValue(p + OpnPrKey::rgRr);
        params.opn.op[op].m_adsrParams.tl = (int)*apvts.getRawParameterValue(p + OpnPrKey::rgTl);
        params.opn.op[op].m_adsrParams.ks = (int)*apvts.getRawParameterValue(p + OpnPrKey::ks);
    }
}
