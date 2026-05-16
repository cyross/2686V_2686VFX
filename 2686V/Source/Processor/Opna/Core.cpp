#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void OpnaProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = OpnaPrKey::prefix;

    // ==========================================
    // OPNA (YM2608) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::alg, code + OpnaPrName::alg, OpnaPrValue::Alg::min, OpnaPrValue::Alg::max, OpnaPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::fb0, code + OpnaPrName::fb0, OpnaPrValue::Fb0::min, OpnaPrValue::Fb0::max, OpnaPrValue::Fb0::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::fb2, code + OpnaPrName::fb2, OpnaPrValue::Fb2::min, OpnaPrValue::Fb2::max, OpnaPrValue::Fb2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::bit, code + OpnaPrName::bit, OpnaPrValue::Bit::min, OpnaPrValue::Bit::max, OpnaPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::rate, code + OpnaPrName::rate, OpnaPrValue::Rate::min, OpnaPrValue::Rate::max, OpnaPrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::N88Lfo::freq, code + OpnaPrName::N88Lfo::freq, OpnaPrValue::Lfo::N88Freq::min, OpnaPrValue::Lfo::N88Freq::max, OpnaPrValue::Lfo::N88Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::N88Lfo::shape, code + OpnaPrName::N88Lfo::shape, OpnaPrValue::Lfo::Shape::min, OpnaPrValue::Lfo::Shape::max, OpnaPrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + OpnaPrKey::N88Lfo::amSmoothRatio, code + OpnaPrName::N88Lfo::amSmoothRatio, OpnaPrValue::Lfo::AmSmRt::min, OpnaPrValue::Lfo::AmSmRt::max, OpnaPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + OpnaPrKey::N88Lfo::am, code + OpnaPrName::N88Lfo::am, OpnaPrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + OpnaPrKey::N88Lfo::pm, code + OpnaPrName::N88Lfo::pm, OpnaPrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::N88Lfo::pms, code + OpnaPrName::N88Lfo::pms, OpnaPrValue::Lfo::N88Pms::min, OpnaPrValue::Lfo::N88Pms::max, OpnaPrValue::Lfo::N88Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::N88Lfo::pmd, code + OpnaPrName::N88Lfo::pmd, OpnaPrValue::Lfo::N88Pmd::min, OpnaPrValue::Lfo::N88Pmd::max, OpnaPrValue::Lfo::N88Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::N88Lfo::amd, code + OpnaPrName::N88Lfo::amd, OpnaPrValue::Lfo::N88Amd::min, OpnaPrValue::Lfo::N88Amd::max, OpnaPrValue::Lfo::N88Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::N88Lfo::syncDelay, code + OpnaPrName::N88Lfo::syncDelay, OpnaPrValue::Lfo::SyncDelay::min, OpnaPrValue::Lfo::SyncDelay::max, OpnaPrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::pan, code + OpnaPrName::pan, OpnaPrValue::Pan::min, OpnaPrValue::Pan::max, OpnaPrValue::Pan::initial));

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        juce::String prefix = code + OpnaPrKey::op + juce::String(op);
        juce::String namePrefix = code + OpnaPrName::op + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::mul, namePrefix + OpnaPrName::mul, OpnaPrValue::Op::Mul::min, OpnaPrValue::Op::Mul::max, OpnaPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::dt, namePrefix + OpnaPrName::dt, OpnaPrValue::Op::Dt::min, OpnaPrValue::Op::Dt::max, OpnaPrValue::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::ks, namePrefix + OpnaPrName::ks, OpnaPrValue::Op::Ks::min, OpnaPrValue::Op::Ks::max, OpnaPrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::se, namePrefix + OpnaPrName::se, OpnaPrValue::Op::Se::min, OpnaPrValue::Op::Se::max, OpnaPrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpnaPrKey::seFreq, namePrefix + OpnaPrName::seFreq, OpnaPrValue::Op::SeFreq::min, OpnaPrValue::Op::SeFreq::max, OpnaPrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::freqs, namePrefix + OpnaPrName::freqs, OpnaPrValue::Op::Lfo::Freqs::min, OpnaPrValue::Op::Lfo::Freqs::max, OpnaPrValue::Op::Lfo::Freqs::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::am, namePrefix + OpnaPrName::am, OpnaPrValue::Op::Lfo::Am::initial)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::vib, namePrefix + OpnaPrName::vib, OpnaPrValue::Op::Lfo::Vib::initial)); // PM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::pms, namePrefix + OpnaPrName::pms, OpnaPrValue::Op::Lfo::Pms::min, OpnaPrValue::Op::Lfo::Pms::max, OpnaPrValue::Op::Lfo::Pms::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::syncDelay, namePrefix + OpnaPrName::syncDelay, OpnaPrValue::Op::Lfo::SyncDelay::min, OpnaPrValue::Op::Lfo::SyncDelay::max, OpnaPrValue::Op::Lfo::SyncDelay::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::ams, namePrefix + OpnaPrName::ams, OpnaPrValue::Op::Lfo::Ams::min, OpnaPrValue::Op::Lfo::Ams::max, OpnaPrValue::Op::Lfo::Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::n88Ams, namePrefix + OpnaPrName::n88Ams, OpnaPrValue::Lfo::N88Ams::min, OpnaPrValue::Lfo::N88Ams::max, OpnaPrValue::Lfo::N88Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::fix, namePrefix + OpnaPrName::fix, OpnaPrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpnaPrKey::fixFreq, namePrefix + OpnaPrName::fixFreq, OpnaPrValue::Op::FixFreq::min, OpnaPrValue::Op::FixFreq::max, OpnaPrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::mask, namePrefix + OpnaPrName::mask, OpnaPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgAr, namePrefix + OpnaPrName::rgAr, OpnaPrValue::Op::RgAdsr::Ar::min, OpnaPrValue::Op::RgAdsr::Ar::max, OpnaPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgDr, namePrefix + OpnaPrName::rgDr, OpnaPrValue::Op::RgAdsr::Dr::min, OpnaPrValue::Op::RgAdsr::Dr::max, OpnaPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgSl, namePrefix + OpnaPrName::rgSl, OpnaPrValue::Op::RgAdsr::Sl::min, OpnaPrValue::Op::RgAdsr::Sl::max, OpnaPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgSr, namePrefix + OpnaPrName::rgSr, OpnaPrValue::Op::RgAdsr::Sr::min, OpnaPrValue::Op::RgAdsr::Sr::max, OpnaPrValue::Op::RgAdsr::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgRr, namePrefix + OpnaPrName::rgRr, OpnaPrValue::Op::RgAdsr::Rr::min, OpnaPrValue::Op::RgAdsr::Rr::max, OpnaPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgTl, namePrefix + OpnaPrName::rgTl, OpnaPrValue::Op::RgAdsr::Tl::min, OpnaPrValue::Op::RgAdsr::Tl::max, OpnaPrValue::Op::RgAdsr::Tl::initial));
    }
}

void OpnaProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = OpnaPrKey::prefix;

    params.opna.algorithm = (int)*apvts.getRawParameterValue(code + OpnaPrKey::alg);
    params.opna.feedback = *apvts.getRawParameterValue(code + OpnaPrKey::fb0);
    params.opna.feedback2 = *apvts.getRawParameterValue(code + OpnaPrKey::fb2);
    params.opna.fmBitDepth = (int)*apvts.getRawParameterValue(code + OpnaPrKey::bit);
    params.opna.fmRateIndex = (int)*apvts.getRawParameterValue(code + OpnaPrKey::rate);
    // Int(0〜16383) として読み込み、Hzに変換する
    float n88Speed = *apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::freq);
    // ソフトウェアLFOの周波数変換公式 (60Hz割り込み、16bitカウンター)
    // f = (Speed * 60Hz) / 65536
    params.opna.lfoFreq = (n88Speed * 60.0f) / 65536.0f;
    params.opna.lfoWave = (int)*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::shape);
    params.opna.lfoAmSmRt = *apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::amSmoothRatio);
    params.opna.amEnable = (*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::am) > OpnaPrValue::boolThread);
    params.opna.pmEnable = (*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::pm) > OpnaPrValue::boolThread);
    params.opna.lfoPms = (int)*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::pms);
    params.opna.lfoPmd = (int)*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::pmd);
    params.opna.lfoAmd = (int)*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::amd);
    params.opna.lfoAms = 0; // グローバルAMSはN88-BASICには存在しないので0固定
    params.opna.lfoSyncDelay = (int)*apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::syncDelay);
    params.opna.pan = (int)*apvts.getRawParameterValue(code + OpnaPrKey::pan);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        juce::String p = code + OpnaPrKey::op + juce::String(op);

        params.opna.op[op].multiple = (int)*apvts.getRawParameterValue(p + OpnaPrKey::mul);
        params.opna.op[op].detune = (int)*apvts.getRawParameterValue(p + OpnaPrKey::dt);
        params.opna.op[op].attack = 0.0f;
        params.opna.op[op].decay = 0.0f;
        params.opna.op[op].sustain = 0.0f;
        params.opna.op[op].release = 0.0f;
        params.opna.op[op].sustainRate = 0.0f;
        params.opna.op[op].totalLevel = 0.0f;
        params.opna.op[op].keyScale = (int)*apvts.getRawParameterValue(p + OpnaPrKey::ks);
        params.opna.op[op].keyScaleLevel = 0;
        params.opna.op[op].phaseOffset = 0.0f;
        params.opna.op[op].ssgEg = (int)*apvts.getRawParameterValue(p + OpnaPrKey::se);
        params.opna.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + OpnaPrKey::seFreq);
        params.opna.op[op].fixedMode = (*apvts.getRawParameterValue(p + OpnaPrKey::fix) > OpnaPrValue::boolThread);
        params.opna.op[op].fixedFreq = *apvts.getRawParameterValue(p + OpnaPrKey::fixFreq);
        params.opna.op[op].waveSelect = 0; // Sine
        params.opna.op[op].lfoFreqIndex = (int)*apvts.getRawParameterValue(p + OpnaPrKey::freqs);
        params.opna.op[op].vibEnable = (*apvts.getRawParameterValue(p + OpnaPrKey::vib) > OpnaPrValue::boolThread);
        params.opna.op[op].pms = (int)*apvts.getRawParameterValue(p + OpnaPrKey::pms);
        params.opna.op[op].amEnable = (*apvts.getRawParameterValue(p + OpnaPrKey::am) > OpnaPrValue::boolThread);
        params.opna.op[op].n88Ams = (int)*apvts.getRawParameterValue(p + OpnaPrKey::n88Ams);
        params.opna.op[op].amd = 0; // ローカルAMDは未使用なので0固定
        params.opna.op[op].egType = true;
        params.opna.op[op].oplAms = 0.0f;
        params.opna.op[op].oplAmd = 0.0f;
        params.opna.op[op].oplPms = 0.0f;
        params.opna.op[op].oplPmd = 0.0f;
        params.opna.op[op].lfoSyncDelay = (int)*apvts.getRawParameterValue(p + OpnaPrKey::syncDelay);
        params.opna.op[op].mask = (*apvts.getRawParameterValue(p + OpnaPrKey::mask) > OpnaPrValue::boolThread);

        params.opna.op[op].isOplMode = false;
        params.opna.op[op].regEnable = true;
        params.opna.op[op].rar = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgAr);
        params.opna.op[op].rdr = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgDr);
        params.opna.op[op].rsl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgSl);
        params.opna.op[op].rsr = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgSr);
        params.opna.op[op].rrr = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgRr);
        params.opna.op[op].rtl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgTl);
    }
}
