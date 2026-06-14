#include "./ProcessorOpn.h"

#include "./ProcessorOpnKeys.h"
#include "./ProcessorOpnValues.h"
#include "./ProcessorOpnNames.h"

void OpnProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = OpnPrKey::prefix;
    const juce::String prefixName = OpnPrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpnPrKey::level, prefixName + OpnPrName::level, OpnPrValue::Level::min, OpnPrValue::Level::max, OpnPrValue::Level::initial));

    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::alg, prefixName + OpnPrName::alg, OpnPrValue::Alg::min, OpnPrValue::Alg::max, OpnPrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::fb, prefixName + OpnPrName::fb, OpnPrValue::Fb::min, OpnPrValue::Fb::max, OpnPrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::bit, prefixName + OpnPrName::bit, OpnPrValue::Bit::min, OpnPrValue::Bit::max, OpnPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::rate, prefixName + OpnPrName::rate, OpnPrValue::Rate::min, OpnPrValue::Rate::max, OpnPrValue::Rate::initial)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::N88Lfo::freq, prefixName + OpnPrName::N88Lfo::freq, OpnPrValue::Lfo::N88Freq::min, OpnPrValue::Lfo::N88Freq::max, OpnPrValue::Lfo::N88Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::N88Lfo::shape, prefixName + OpnPrName::N88Lfo::shape, OpnPrValue::Lfo::Shape::min, OpnPrValue::Lfo::Shape::max, OpnPrValue::Lfo::Shape::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpnPrKey::N88Lfo::amSmoothRatio, prefixName + OpnPrName::N88Lfo::amSmoothRatio, OpnPrValue::Lfo::AmSmRt::min, OpnPrValue::Lfo::AmSmRt::max, OpnPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnPrKey::N88Lfo::am, prefixName + OpnPrName::N88Lfo::am, OpnPrValue::Lfo::Am::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnPrKey::N88Lfo::pm, prefixName + OpnPrName::N88Lfo::pm, OpnPrValue::Lfo::Pm::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::N88Lfo::pms, prefixName + OpnPrName::N88Lfo::pms, OpnPrValue::Lfo::N88Pms::min, OpnPrValue::Lfo::N88Pms::max, OpnPrValue::Lfo::N88Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::N88Lfo::pmd, prefixName + OpnPrName::N88Lfo::pmd, OpnPrValue::Lfo::N88Pmd::min, OpnPrValue::Lfo::N88Pmd::max, OpnPrValue::Lfo::N88Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::N88Lfo::amd, prefixName + OpnPrName::N88Lfo::amd, OpnPrValue::Lfo::N88Amd::min, OpnPrValue::Lfo::N88Amd::max, OpnPrValue::Lfo::N88Amd::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::N88Lfo::syncDelay, prefixName + OpnPrName::N88Lfo::syncDelay, OpnPrValue::Lfo::SyncDelay::min, OpnPrValue::Lfo::SyncDelay::max, OpnPrValue::Lfo::SyncDelay::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::Unison::voices, prefixName + OpnPrName::Unison::voices, OpnPrValue::Unison::Voices::min, OpnPrValue::Unison::Voices::max, OpnPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + OpnPrKey::Unison::detune, prefixName + OpnPrName::Unison::detune, OpnPrValue::Unison::Detune::min, OpnPrValue::Unison::Detune::max, OpnPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + OpnPrKey::Unison::spread, prefixName + OpnPrName::Unison::spread, OpnPrValue::Unison::Spread::min, OpnPrValue::Unison::Spread::max, OpnPrValue::Unison::Spread::initial));

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + OpnPrKey::op + juce::String(op);
        juce::String opPrefixName = prefixName + OpnPrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::mul, opPrefixName + OpnPrName::mul, OpnPrValue::Op::Mul::min, OpnPrValue::Op::Mul::max, OpnPrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::dt, opPrefixName + OpnPrName::dt, OpnPrValue::Op::Dt::min, OpnPrValue::Op::Dt::max, OpnPrValue::Op::Dt::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::ks, opPrefixName + OpnPrName::ks, OpnPrValue::Op::Ks::min, OpnPrValue::Op::Ks::max, OpnPrValue::Op::Ks::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::fix, opPrefixName + OpnPrName::fix, OpnPrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + OpnPrKey::fixFreq, opPrefixName + OpnPrName::fixFreq, OpnPrValue::Op::FixFreq::min, OpnPrValue::Op::FixFreq::max, OpnPrValue::Op::FixFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::n88Ams, opPrefixName + OpnPrName::n88Ams, OpnPrValue::Lfo::N88Ams::min, OpnPrValue::Lfo::N88Ams::max, OpnPrValue::Lfo::N88Ams::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::mask, opPrefixName + OpnPrName::mask, OpnPrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::PitchAdsr::enable, opPrefixName + OpnPrName::PitchAdsr::enable, OpnPrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::SsgSwEnv::enable, opPrefixName + OpnPrName::SsgSwEnv::enable, OpnPrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::rgAr, opPrefixName + OpnPrName::rgAr, OpnPrValue::Op::RgAdsr::Ar::min, OpnPrValue::Op::RgAdsr::Ar::max, OpnPrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::rgDr, opPrefixName + OpnPrName::rgDr, OpnPrValue::Op::RgAdsr::Dr::min, OpnPrValue::Op::RgAdsr::Dr::max, OpnPrValue::Op::RgAdsr::Dr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::rgSl, opPrefixName + OpnPrName::rgSl, OpnPrValue::Op::RgAdsr::Sl::min, OpnPrValue::Op::RgAdsr::Sl::max, OpnPrValue::Op::RgAdsr::Sl::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::rgSr, opPrefixName + OpnPrName::rgSr, OpnPrValue::Op::RgAdsr::Sr::min, OpnPrValue::Op::RgAdsr::Sr::max, OpnPrValue::Op::RgAdsr::Sr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::rgRr, opPrefixName + OpnPrName::rgRr, OpnPrValue::Op::RgAdsr::Rr::min, OpnPrValue::Op::RgAdsr::Rr::max, OpnPrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + OpnPrKey::rgTl, opPrefixName + OpnPrName::rgTl, OpnPrValue::Op::RgAdsr::Tl::min, OpnPrValue::Op::RgAdsr::Tl::max, OpnPrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::xof, opPrefixName + OpnPrName::xof, OpnPrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::kor, opPrefixName + OpnPrName::kor, OpnPrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + OpnPrKey::ampBypass, opPrefixName + OpnPrName::ampBypass, OpnPrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
    }
}

void OpnProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = OpnPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + OpnPrKey::level);

    pAlg = apvts.getRawParameterValue(prefix + OpnPrKey::alg);
    pFb = apvts.getRawParameterValue(prefix + OpnPrKey::fb);
    pDepth = apvts.getRawParameterValue(prefix + OpnPrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + OpnPrKey::rate);

    pN88LfoFreq = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::freq);
    pN88LfoSyncDelay = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::syncDelay);
    pN88LfoShape = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::shape);
    pN88LfoPmEnable = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::pm);
    pN88LfoPmd = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::pmd);
    pN88LfoPms = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::pms);
    pN88LfoAmEnable = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::am);
    pN88LfoAmSmRt = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::amSmoothRatio);
    pN88LfoAmd = apvts.getRawParameterValue(prefix + OpnPrKey::N88Lfo::amd);

    pUnisonVoices = apvts.getRawParameterValue(prefix + OpnPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + OpnPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + OpnPrKey::Unison::spread);

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        juce::String p = prefix + OpnPrKey::op + juce::String(op);

        pOpMultiple[op] = apvts.getRawParameterValue(p + OpnPrKey::mul);
        pOpDetune[op] = apvts.getRawParameterValue(p + OpnPrKey::dt);

        pOpAdsrBypass[op] = apvts.getRawParameterValue(p + OpnPrKey::ampBypass);
        pOpAdsrRgAr[op] = apvts.getRawParameterValue(p + OpnPrKey::rgAr);
        pOpAdsrRgD1r[op] = apvts.getRawParameterValue(p + OpnPrKey::rgDr);
        pOpAdsrRgD1l[op] = apvts.getRawParameterValue(p + OpnPrKey::rgSl);
        pOpAdsrRgD2r[op] = apvts.getRawParameterValue(p + OpnPrKey::rgSr);
        pOpAdsrRgRr[op] = apvts.getRawParameterValue(p + OpnPrKey::rgRr);
        pOpAdsrRgTl[op] = apvts.getRawParameterValue(p + OpnPrKey::rgTl);
        pOpAdsrKs[op] = apvts.getRawParameterValue(p + OpnPrKey::ks);
        pOpAdsrXof[op] = apvts.getRawParameterValue(p + OpnPrKey::xof);
        pOpAdsrKor[op] = apvts.getRawParameterValue(p + OpnPrKey::kor);

        pOpFixEnable[op] = apvts.getRawParameterValue(p + OpnPrKey::fix);
        pOpFixFreq[op] = apvts.getRawParameterValue(p + OpnPrKey::fixFreq);

        pOpN88LfoAms[op] = apvts.getRawParameterValue(p + OpnPrKey::n88Ams);

        pOpPitchEnvEnable[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::enable);
        pOpPitchEnvAr[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::ar);
        pOpPitchEnvDr[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::dr);
        pOpPitchEnvRr[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::rr);
        pOpPitchEnvStl[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::stl);
        pOpPitchEnvAtl[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::atl);
        pOpPitchEnvSsl[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::ssl);
        pOpPitchEnvRll[op] = apvts.getRawParameterValue(p + OpnPrKey::PitchAdsr::rll);

        pOpSsgSwEnvEnable[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::enable);
        pOpSsgSwEnvSteps[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::steps);
        pOpSsgSwEnvLoop[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::loop);
        pOpSsgSwEnvLoopTo[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::loopTo);
        pOpSsgSwEnvLoopCount[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::loopCount);
        pOpSsgSwEnvStl[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::stl);
        pOpSsgSwEnvR1[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::r1);
        pOpSsgSwEnvL1[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::l1);
        pOpSsgSwEnvR2[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::r2);
        pOpSsgSwEnvL2[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::l2);
        pOpSsgSwEnvR3[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::r3);
        pOpSsgSwEnvL3[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::l3);
        pOpSsgSwEnvR4[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::r4);
        pOpSsgSwEnvL4[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::l4);
        pOpSsgSwEnvR5[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::r5);
        pOpSsgSwEnvL5[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::l5);
        pOpSsgSwEnvR6[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::r6);
        pOpSsgSwEnvL6[op] = apvts.getRawParameterValue(p + OpnPrKey::SsgSwEnv::l6);

        pOpMask[op] = apvts.getRawParameterValue(p + OpnPrKey::mask);
    }
}

void OpnProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.opn.level = pLevel->load(std::memory_order_relaxed);

    params.opn.algorithm = (int)pAlg->load(std::memory_order_relaxed);

    params.opn.feedback = pFb->load(std::memory_order_relaxed);

    params.opn.fmBitDepth = (int)pDepth->load(std::memory_order_relaxed);
    params.opn.fmRateIndex = (int)pRate->load(std::memory_order_relaxed);

    // Int(0〜16383) として読み込み、Hzに変換する
    float n88Speed = pN88LfoFreq->load(std::memory_order_relaxed);
    // ソフトウェアLFOの周波数変換公式 (60Hz割り込み、16bitカウンター)
    // f = (Speed * 60Hz) / 65536
    params.opn.lfoFreq = (n88Speed * 60.0f) / 65536.0f;
    params.opn.lfoWave = (int)pN88LfoShape->load(std::memory_order_relaxed);
    params.opn.lfoAmSmRt = pN88LfoAmSmRt->load(std::memory_order_relaxed);
    params.opn.amEnable = (pN88LfoAmEnable->load(std::memory_order_relaxed) > OpnPrValue::boolThread);
    params.opn.pmEnable = (pN88LfoPmEnable->load(std::memory_order_relaxed) > OpnPrValue::boolThread);
    params.opn.lfoPms = (int)pN88LfoPms->load(std::memory_order_relaxed);
    params.opn.lfoPmd = (int)pN88LfoPmd->load(std::memory_order_relaxed);
    params.opn.lfoAmd = (int)pN88LfoAmd->load(std::memory_order_relaxed);
    params.opn.lfoAms = 0; // グローバルAMSはN88-BASICには存在しないので0固定
    params.opn.lfoSyncDelay = (int)pN88LfoSyncDelay->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.opn.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.opn.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.opn.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int op = 0; op < OpnPrValue::ops; ++op)
    {
        params.opn.op[op].multiple = (int)pOpMultiple[op]->load(std::memory_order_relaxed);
        params.opn.op[op].detune = (int)pOpDetune[op]->load(std::memory_order_relaxed);

        params.opn.op[op].fixedMode = (pOpFixEnable[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread);
        params.opn.op[op].fixedFreq = pOpFixFreq[op]->load(std::memory_order_relaxed);

        params.opn.op[op].waveSelect = 0; // Sine

        params.opn.op[op].n88Ams = (int)pOpN88LfoAms[op]->load(std::memory_order_relaxed);

        params.opn.op[op].mask = (pOpMask[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread);

        params.opn.op[op].m_adsrParams.ar = (int)pOpAdsrRgAr[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.dr = (int)pOpAdsrRgD1r[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.sl = (int)pOpAdsrRgD1l[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.sr = (int)pOpAdsrRgD2r[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.rr = (int)pOpAdsrRgRr[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.tl = (int)pOpAdsrRgTl[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.ks = (int)pOpAdsrKs[op]->load(std::memory_order_relaxed);
        params.opn.op[op].m_adsrParams.xof = (pOpAdsrXof[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread);
        params.opn.op[op].m_adsrParams.kor = pOpAdsrKor[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread;
        params.opn.op[op].m_adsrParams.bypass = pOpAdsrBypass[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread;

        params.opn.op[op].pitchEnvEnable = pOpPitchEnvEnable[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread;
        params.opn.op[op].pitchAdsr.bypass = false;
        params.opn.op[op].pitchAdsr.ar = pOpPitchEnvAr[op]->load(std::memory_order_relaxed);
        params.opn.op[op].pitchAdsr.dr = pOpPitchEnvDr[op]->load(std::memory_order_relaxed);
        params.opn.op[op].pitchAdsr.rr = pOpPitchEnvRr[op]->load(std::memory_order_relaxed);
        params.opn.op[op].pitchAdsr.stl = (int)pOpPitchEnvStl[op]->load(std::memory_order_relaxed);
        params.opn.op[op].pitchAdsr.atl = (int)pOpPitchEnvAtl[op]->load(std::memory_order_relaxed);
        params.opn.op[op].pitchAdsr.ssl = (int)pOpPitchEnvSsl[op]->load(std::memory_order_relaxed);
        params.opn.op[op].pitchAdsr.rll = (int)pOpPitchEnvRll[op]->load(std::memory_order_relaxed);

        params.opn.op[op].ssgEnvEnable = (pOpSsgSwEnvEnable[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread);
        params.opn.op[op].ssgSwEnv.bypass = false;
        params.opn.op[op].ssgSwEnv.steps = (int)pOpSsgSwEnvSteps[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.loop = (pOpSsgSwEnvLoop[op]->load(std::memory_order_relaxed) > OpnPrValue::boolThread);
        params.opn.op[op].ssgSwEnv.loopTo = (int)pOpSsgSwEnvLoopTo[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.loopCount = (int)pOpSsgSwEnvLoopCount[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.stl = pOpSsgSwEnvStl[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.r1 = pOpSsgSwEnvR1[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.l1 = pOpSsgSwEnvL1[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.r2 = pOpSsgSwEnvR2[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.l2 = pOpSsgSwEnvL2[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.r3 = pOpSsgSwEnvR3[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.l3 = pOpSsgSwEnvL3[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.r4 = pOpSsgSwEnvR4[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.l4 = pOpSsgSwEnvL4[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.r5 = pOpSsgSwEnvR5[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.l5 = pOpSsgSwEnvL5[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.r6 = pOpSsgSwEnvR6[op]->load(std::memory_order_relaxed);
        params.opn.op[op].ssgSwEnv.l6 = pOpSsgSwEnvL6[op]->load(std::memory_order_relaxed);
    }
}
