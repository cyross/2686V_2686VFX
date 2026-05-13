#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::alg, code + PrName::alg, PrValue::Alg::min, PrValue::Alg::max, PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb0, code + PrName::fb0, PrValue::Fb0::min, PrValue::Fb0::max, PrValue::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::fb2, code + PrName::fb2, PrValue::Fb2::min, PrValue::Fb2::max, PrValue::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::bit, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::N88Lfo::freq, code + PrName::N88Lfo::freq, PrValue::Lfo::N88Freq::min, PrValue::Lfo::N88Freq::max, PrValue::Lfo::N88Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::N88Lfo::shape, code + PrName::N88Lfo::shape, PrValue::Lfo::Shape::min, PrValue::Lfo::Shape::max, PrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::N88Lfo::amSmoothRatio, code + PrName::N88Lfo::amSmoothRatio, PrValue::Lfo::AmSmRt::min, PrValue::Lfo::AmSmRt::max, PrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::N88Lfo::am, code + PrName::N88Lfo::am, PrValue::Lfo::Am::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::N88Lfo::pm, code + PrName::N88Lfo::pm, PrValue::Lfo::Pm::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::N88Lfo::pms, code + PrName::N88Lfo::pms, PrValue::Lfo::N88Pms::min, PrValue::Lfo::N88Pms::max, PrValue::Lfo::N88Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::N88Lfo::pmd, code + PrName::N88Lfo::pmd, PrValue::Lfo::N88Pmd::min, PrValue::Lfo::N88Pmd::max, PrValue::Lfo::N88Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::N88Lfo::amd, code + PrName::N88Lfo::amd, PrValue::Lfo::N88Amd::min, PrValue::Lfo::N88Amd::max, PrValue::Lfo::N88Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::N88Lfo::syncDelay, code + PrName::N88Lfo::syncDelay, PrValue::Lfo::SyncDelay::min, PrValue::Lfo::SyncDelay::max, PrValue::Lfo::SyncDelay::initial));

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String prefix = code + PrKey::op + juce::String(op);
        juce::String namePrefix = code + PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::mul, namePrefix + PrName::mul, PrValue::Op::Mul::min, PrValue::Op::Mul::max, PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::dt, namePrefix + PrName::dt, PrValue::Op::Dt::min, PrValue::Op::Dt::max, PrValue::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::ks, namePrefix + PrName::ks, PrValue::Op::Ks::min, PrValue::Op::Ks::max, PrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::fix, namePrefix + PrName::fix, PrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::fixFreq, namePrefix + PrName::fixFreq, PrValue::Op::FixFreq::min, PrValue::Op::FixFreq::max, PrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::n88Ams, namePrefix + PrName::n88Ams, PrValue::Lfo::N88Ams::min, PrValue::Lfo::N88Ams::max, PrValue::Lfo::N88Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::mask, namePrefix + PrName::mask, PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgAr, namePrefix + PrName::rgAr, PrValue::Op::RgAdsr::Ar::min, PrValue::Op::RgAdsr::Ar::max, PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgDr, namePrefix + PrName::rgDr, PrValue::Op::RgAdsr::Dr::min, PrValue::Op::RgAdsr::Dr::max, PrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgSl, namePrefix + PrName::rgSl, PrValue::Op::RgAdsr::Sl::min, PrValue::Op::RgAdsr::Sl::max, PrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgSr, namePrefix + PrName::rgSr, PrValue::Op::RgAdsr::Sr::min, PrValue::Op::RgAdsr::Sr::max, PrValue::Op::RgAdsr::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgRr, namePrefix + PrName::rgRr, PrValue::Op::RgAdsr::Rr::min, PrValue::Op::RgAdsr::Rr::max, PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::rgTl, namePrefix + PrName::rgTl, PrValue::Op::RgAdsr::Tl::min, PrValue::Op::RgAdsr::Tl::max, PrValue::Op::RgAdsr::Tl::initial));
    }
}

void OpnProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    params.opn.algorithm = (int)*apvts.getRawParameterValue(code + PrKey::alg);
    params.opn.feedback = *apvts.getRawParameterValue(code + PrKey::fb0);
    params.opn.feedback2 = *apvts.getRawParameterValue(code + PrKey::fb2);
    params.opn.fmBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::bit);
    params.opn.fmRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);
    // Int(0〜16383) として読み込み、Hzに変換する
    float n88Speed = *apvts.getRawParameterValue(code + PrKey::N88Lfo::freq);
    // ソフトウェアLFOの周波数変換公式 (60Hz割り込み、16bitカウンター)
    // f = (Speed * 60Hz) / 65536
    params.opn.lfoFreq = (n88Speed * 60.0f) / 65536.0f;
    params.opn.lfoWave = (int)*apvts.getRawParameterValue(code + PrKey::N88Lfo::shape);
    params.opn.lfoAmSmRt = *apvts.getRawParameterValue(code + PrKey::N88Lfo::amSmoothRatio);
    params.opn.amEnable = (*apvts.getRawParameterValue(code + PrKey::N88Lfo::am) > PrValue::boolThread);
    params.opn.pmEnable = (*apvts.getRawParameterValue(code + PrKey::N88Lfo::pm) > PrValue::boolThread);
    params.opn.lfoPms = (int)*apvts.getRawParameterValue(code + PrKey::N88Lfo::pms);
    params.opn.lfoPmd = (int)*apvts.getRawParameterValue(code + PrKey::N88Lfo::pmd);
    params.opn.lfoAmd = (int)*apvts.getRawParameterValue(code + PrKey::N88Lfo::amd);
    params.opn.lfoAms = 0; // グローバルAMSはN88-BASICには存在しないので0固定
    params.opn.lfoSyncDelay = (int)*apvts.getRawParameterValue(code + PrKey::N88Lfo::syncDelay);

    for (int op = 0; op < PrValue::ops; ++op)
    {
        juce::String p = code + PrKey::op + juce::String(op);

        params.opn.op[op].multiple = (int)*apvts.getRawParameterValue(p + PrKey::mul);
        params.opn.op[op].detune = (int)*apvts.getRawParameterValue(p + PrKey::dt);
        params.opn.op[op].attack = 0.0f;
        params.opn.op[op].decay = 0.0f;
        params.opn.op[op].sustain = 0.0f;
        params.opn.op[op].release = 0.0f;
        params.opn.op[op].sustainRate = 0.0f;
        params.opn.op[op].totalLevel = 0.0f;
        params.opn.op[op].keyScale = (int)*apvts.getRawParameterValue(p + PrKey::ks);
        params.opn.op[op].keyScaleLevel = 0;
        params.opn.op[op].phaseOffset = 0.0f;
        params.opn.op[op].ssgEg = 0;
        params.opn.op[op].fmSsgEgFreq = 0.0f;
        params.opn.op[op].fixedMode = (*apvts.getRawParameterValue(p + PrKey::fix) > PrValue::boolThread);
        params.opn.op[op].fixedFreq = *apvts.getRawParameterValue(p + PrKey::fixFreq);
        params.opn.op[op].waveSelect = 0; // Sine
        params.opn.op[op].vibEnable = false;
        params.opn.op[op].pms = 0.0f;
        params.opn.op[op].amEnable = false; // OPNにはAMは無い
        params.opn.op[op].ams = (int)*apvts.getRawParameterValue(p + PrKey::n88Ams);
        params.opn.op[op].amd = 0; // ローカルAMDは未使用なので0固定
        params.opn.op[op].oplAms = 0.0f;
        params.opn.op[op].oplAmd = 0.0f;
        params.opn.op[op].oplPms = 0.0f;
        params.opn.op[op].oplPmd = 0.0f;
        params.opn.op[op].egType = true;
        params.opn.op[op].lfoSyncDelay = 0;
        params.opn.op[op].mask = (*apvts.getRawParameterValue(p + PrKey::mask) > PrValue::boolThread);

        params.opn.op[op].isOplMode = false;
        params.opn.op[op].regEnable = true;
        params.opn.op[op].rar = (int)*apvts.getRawParameterValue(p + PrKey::rgAr);
        params.opn.op[op].rdr = (int)*apvts.getRawParameterValue(p + PrKey::rgDr);
        params.opn.op[op].rsl = (int)*apvts.getRawParameterValue(p + PrKey::rgSl);
        params.opn.op[op].rsr = (int)*apvts.getRawParameterValue(p + PrKey::rgSr);
        params.opn.op[op].rrr = (int)*apvts.getRawParameterValue(p + PrKey::rgRr);
        params.opn.op[op].rtl = (int)*apvts.getRawParameterValue(p + PrKey::rgTl);
    }
}
