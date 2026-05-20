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
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::fb, code + OpnaPrName::fb, OpnaPrValue::Fb::min, OpnaPrValue::Fb::max, OpnaPrValue::Fb::initial));
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

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::PitchAdsr::enable, namePrefix + OpnaPrName::PitchAdsr::enable, OpnaPrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::SsgSwEnv::enable, namePrefix + OpnaPrName::SsgSwEnv::enable, OpnaPrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgAr, namePrefix + OpnaPrName::rgAr, OpnaPrValue::Op::RgAdsr::Ar::min, OpnaPrValue::Op::RgAdsr::Ar::max, OpnaPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgDr, namePrefix + OpnaPrName::rgDr, OpnaPrValue::Op::RgAdsr::Dr::min, OpnaPrValue::Op::RgAdsr::Dr::max, OpnaPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgSl, namePrefix + OpnaPrName::rgSl, OpnaPrValue::Op::RgAdsr::Sl::min, OpnaPrValue::Op::RgAdsr::Sl::max, OpnaPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgSr, namePrefix + OpnaPrName::rgSr, OpnaPrValue::Op::RgAdsr::Sr::min, OpnaPrValue::Op::RgAdsr::Sr::max, OpnaPrValue::Op::RgAdsr::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgRr, namePrefix + OpnaPrName::rgRr, OpnaPrValue::Op::RgAdsr::Rr::min, OpnaPrValue::Op::RgAdsr::Rr::max, OpnaPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnaPrKey::rgTl, namePrefix + OpnaPrName::rgTl, OpnaPrValue::Op::RgAdsr::Tl::min, OpnaPrValue::Op::RgAdsr::Tl::max, OpnaPrValue::Op::RgAdsr::Tl::initial));

        addOpPitchEnvParameters(layout, prefix, namePrefix);
        addOpSsgSwEnvParameters(layout, prefix, namePrefix);
    }
}

void OpnaProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = OpnaPrKey::prefix;

    params.opna.algorithm = (int)*apvts.getRawParameterValue(code + OpnaPrKey::alg);
    params.opna.feedback = *apvts.getRawParameterValue(code + OpnaPrKey::fb);
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
        params.opna.op[op].ssgEg = (int)*apvts.getRawParameterValue(p + OpnaPrKey::se);
        params.opna.op[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + OpnaPrKey::seFreq);
        params.opna.op[op].fixedMode = (*apvts.getRawParameterValue(p + OpnaPrKey::fix) > OpnaPrValue::boolThread);
        params.opna.op[op].fixedFreq = *apvts.getRawParameterValue(p + OpnaPrKey::fixFreq);
        params.opna.op[op].waveSelect = 0; // Sine
        params.opna.op[op].lfoFreqIndex = (int)*apvts.getRawParameterValue(p + OpnaPrKey::freqs);
        params.opna.op[op].vibEnable = (*apvts.getRawParameterValue(p + OpnaPrKey::vib) > OpnaPrValue::boolThread);
        params.opna.op[op].pms = (int)*apvts.getRawParameterValue(p + OpnaPrKey::pms);
        params.opna.op[op].amEnable = (*apvts.getRawParameterValue(p + OpnaPrKey::am) > OpnaPrValue::boolThread);
        params.opna.op[op].ams = (int)*apvts.getRawParameterValue(p + OpnaPrKey::ams);
        params.opna.op[op].n88Ams = (int)*apvts.getRawParameterValue(p + OpnaPrKey::n88Ams);
        params.opna.op[op].amd = 0; // ローカルAMDは未使用なので0固定
        params.opna.op[op].egType = true;
        params.opna.op[op].lfoSyncDelay = (int)*apvts.getRawParameterValue(p + OpnaPrKey::syncDelay);
        params.opna.op[op].mask = (*apvts.getRawParameterValue(p + OpnaPrKey::mask) > OpnaPrValue::boolThread);

        params.opna.op[op].m_adsrParams.ar = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgAr);
        params.opna.op[op].m_adsrParams.dr = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgDr);
        params.opna.op[op].m_adsrParams.sl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgSl);
        params.opna.op[op].m_adsrParams.sr = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgSr);
        params.opna.op[op].m_adsrParams.rr = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgRr);
        params.opna.op[op].m_adsrParams.tl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::rgTl);
        params.opna.op[op].m_adsrParams.ks = (int)*apvts.getRawParameterValue(p + OpnaPrKey::ks);

        params.opna.op[op].pitchEnvEnable = (*apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::enable) > OpnaPrValue::boolThread);
        params.opna.op[op].pitchAdsr.bypass = false;
        params.opna.op[op].pitchAdsr.ar = *apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::ar);
        params.opna.op[op].pitchAdsr.dr = *apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::dr);
        params.opna.op[op].pitchAdsr.rr = *apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::rr);
        params.opna.op[op].pitchAdsr.stl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::stl);
        params.opna.op[op].pitchAdsr.atl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::atl);
        params.opna.op[op].pitchAdsr.ssl = (int)*apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::ssl);
        params.opna.op[op].pitchAdsr.rll = (int)*apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::rll);

        params.opna.op[op].ssgEnvEnable = (*apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::enable) > OpnaPrValue::boolThread);
        params.opna.op[op].ssgSwEnv.bypass = false;
        params.opna.op[op].ssgSwEnv.steps = (int)*apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::steps);
        params.opna.op[op].ssgSwEnv.loop = (*apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::loop) > OpnaPrValue::boolThread);
        params.opna.op[op].ssgSwEnv.loopTo = (int)*apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::loopTo);
        params.opna.op[op].ssgSwEnv.loopCount = (int)*apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::loopCount);
        params.opna.op[op].ssgSwEnv.stl = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::stl);
        params.opna.op[op].ssgSwEnv.r1 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r1);
        params.opna.op[op].ssgSwEnv.l1 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l1);
        params.opna.op[op].ssgSwEnv.r2 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r2);
        params.opna.op[op].ssgSwEnv.l2 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l2);
        params.opna.op[op].ssgSwEnv.r3 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r3);
        params.opna.op[op].ssgSwEnv.l3 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l3);
        params.opna.op[op].ssgSwEnv.r4 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r4);
        params.opna.op[op].ssgSwEnv.l4 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l4);
        params.opna.op[op].ssgSwEnv.r5 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r5);
        params.opna.op[op].ssgSwEnv.l5 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l5);
        params.opna.op[op].ssgSwEnv.r6 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r6);
        params.opna.op[op].ssgSwEnv.l6 = *apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l6);
    }
}
