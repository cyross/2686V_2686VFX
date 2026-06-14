#include "./ProcessorOpna.h"

#include "./ProcessorOpnaKeys.h"
#include "./ProcessorOpnaValues.h"
#include "./ProcessorOpnaNames.h"

void OpnaProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = OpnaPrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + OpnaPrKey::level, code + OpnaPrName::level, OpnaPrValue::Level::min, OpnaPrValue::Level::max, OpnaPrValue::Level::initial));

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

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::Unison::voices, code + OpnaPrName::Unison::voices, OpnaPrValue::Unison::Voices::min, OpnaPrValue::Unison::Voices::max, OpnaPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + OpnaPrKey::Unison::detune, code + OpnaPrName::Unison::detune, OpnaPrValue::Unison::Detune::min, OpnaPrValue::Unison::Detune::max, OpnaPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + OpnaPrKey::Unison::spread, code + OpnaPrName::Unison::spread, OpnaPrValue::Unison::Spread::min, OpnaPrValue::Unison::Spread::max, OpnaPrValue::Unison::Spread::initial));

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

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::xof, namePrefix + OpnaPrName::xof, OpnaPrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::kor, namePrefix + OpnaPrName::kor, OpnaPrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + OpnaPrKey::ampBypass, namePrefix + OpnaPrName::ampBypass, OpnaPrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
        addOpSsgSwEnvParameters(layout, prefix, namePrefix);
    }
}

void OpnaProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = OpnaPrKey::prefix;

    pLevel = apvts.getRawParameterValue(code + OpnaPrKey::level);

    pAlg = apvts.getRawParameterValue(code + OpnaPrKey::alg);
    pFb = apvts.getRawParameterValue(code + OpnaPrKey::fb);
    pDepth = apvts.getRawParameterValue(code + OpnaPrKey::bit);
    pRate = apvts.getRawParameterValue(code + OpnaPrKey::rate);
    pPan = apvts.getRawParameterValue(code + OpnaPrKey::pan);

    pN88LfoFreq = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::freq);
    pN88LfoSyncDelay = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::syncDelay);
    pN88LfoShape = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::shape);
    pN88LfoPmEnable = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::pm);
    pN88LfoPmd = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::pmd);
    pN88LfoPms = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::pms);
    pN88LfoAmEnable = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::am);
    pN88LfoAmSmRt = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::amSmoothRatio);
    pN88LfoAmd = apvts.getRawParameterValue(code + OpnaPrKey::N88Lfo::amd);

    pUnisonVoices = apvts.getRawParameterValue(code + OpnaPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(code + OpnaPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(code + OpnaPrKey::Unison::spread);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        juce::String p = code + OpnaPrKey::op + juce::String(op);

        pOpMultiple[op] = apvts.getRawParameterValue(p + OpnaPrKey::mul);
        pOpDetune[op] = apvts.getRawParameterValue(p + OpnaPrKey::dt);

        pOpAdsrBypass[op] = apvts.getRawParameterValue(p + OpnaPrKey::ampBypass);
        pOpAdsrRgAr[op] = apvts.getRawParameterValue(p + OpnaPrKey::rgAr);
        pOpAdsrRgD1r[op] = apvts.getRawParameterValue(p + OpnaPrKey::rgDr);
        pOpAdsrRgD1l[op] = apvts.getRawParameterValue(p + OpnaPrKey::rgSl);
        pOpAdsrRgD2r[op] = apvts.getRawParameterValue(p + OpnaPrKey::rgSr);
        pOpAdsrRgRr[op] = apvts.getRawParameterValue(p + OpnaPrKey::rgRr);
        pOpAdsrRgTl[op] = apvts.getRawParameterValue(p + OpnaPrKey::rgTl);
        pOpAdsrKs[op] = apvts.getRawParameterValue(p + OpnaPrKey::ks);
        pOpAdsrXof[op] = apvts.getRawParameterValue(p + OpnaPrKey::xof);
        pOpAdsrKor[op] = apvts.getRawParameterValue(p + OpnaPrKey::kor);

        pOpSsgEg[op] = apvts.getRawParameterValue(p + OpnaPrKey::se);
        pOpSsgEgFreq[op] = apvts.getRawParameterValue(p + OpnaPrKey::seFreq);

        pOpFixEnable[op] = apvts.getRawParameterValue(p + OpnaPrKey::fix);
        pOpFixFreq[op] = apvts.getRawParameterValue(p + OpnaPrKey::fixFreq);

        pOpLfoFreqs[op] = apvts.getRawParameterValue(p + OpnaPrKey::freqs);
        pOpLfoSyncDelay[op] = apvts.getRawParameterValue(p + OpnaPrKey::syncDelay);
        pOpLfoPmEnable[op] = apvts.getRawParameterValue(p + OpnaPrKey::vib);
        pOpLfoPms[op] = apvts.getRawParameterValue(p + OpnaPrKey::pms);
        pOpLfoAmEnable[op] = apvts.getRawParameterValue(p + OpnaPrKey::am);
        pOpLfoAms[op] = apvts.getRawParameterValue(p + OpnaPrKey::ams);

        pOpN88LfoAms[op] = apvts.getRawParameterValue(p + OpnaPrKey::n88Ams);

        pOpPitchEnvEnable[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::enable);
        pOpPitchEnvAr[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::ar);
        pOpPitchEnvDr[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::dr);
        pOpPitchEnvRr[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::rr);
        pOpPitchEnvStl[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::stl);
        pOpPitchEnvAtl[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::atl);
        pOpPitchEnvSsl[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::ssl);
        pOpPitchEnvRll[op] = apvts.getRawParameterValue(p + OpnaPrKey::PitchAdsr::rll);

        pOpSsgSwEnvEnable[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::enable);
        pOpSsgSwEnvSteps[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::steps);
        pOpSsgSwEnvLoop[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::loop);
        pOpSsgSwEnvLoopTo[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::loopTo);
        pOpSsgSwEnvLoopCount[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::loopCount);
        pOpSsgSwEnvStl[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::stl);
        pOpSsgSwEnvR1[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r1);
        pOpSsgSwEnvL1[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l1);
        pOpSsgSwEnvR2[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r2);
        pOpSsgSwEnvL2[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l2);
        pOpSsgSwEnvR3[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r3);
        pOpSsgSwEnvL3[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l3);
        pOpSsgSwEnvR4[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r4);
        pOpSsgSwEnvL4[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l4);
        pOpSsgSwEnvR5[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r5);
        pOpSsgSwEnvL5[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l5);
        pOpSsgSwEnvR6[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::r6);
        pOpSsgSwEnvL6[op] = apvts.getRawParameterValue(p + OpnaPrKey::SsgSwEnv::l6);

        pOpMask[op] = apvts.getRawParameterValue(p + OpnaPrKey::mask);
    }
}

void OpnaProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.opna.level = pLevel->load(std::memory_order_relaxed);

    params.opna.algorithm = (int)pAlg->load(std::memory_order_relaxed);

    params.opna.feedback = pFb->load(std::memory_order_relaxed);

    params.opna.fmBitDepth = (int)pDepth->load(std::memory_order_relaxed);
    params.opna.fmRateIndex = (int)pRate->load(std::memory_order_relaxed);

    // Int(0〜16383) として読み込み、Hzに変換する
    float n88Speed = pN88LfoFreq->load(std::memory_order_relaxed);
    // ソフトウェアLFOの周波数変換公式 (60Hz割り込み、16bitカウンター)
    // f = (Speed * 60Hz) / 65536
    params.opna.lfoFreq = (n88Speed * 60.0f) / 65536.0f;
    params.opna.lfoWave = (int)pN88LfoShape->load(std::memory_order_relaxed);
    params.opna.lfoAmSmRt = pN88LfoAmSmRt->load(std::memory_order_relaxed);
    params.opna.amEnable = (pN88LfoAmEnable->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
    params.opna.pmEnable = (pN88LfoPmEnable->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
    params.opna.lfoPms = (int)pN88LfoPms->load(std::memory_order_relaxed);
    params.opna.lfoPmd = (int)pN88LfoPmd->load(std::memory_order_relaxed);
    params.opna.lfoAmd = (int)pN88LfoAmd->load(std::memory_order_relaxed);
    params.opna.lfoAms = 0; // グローバルAMSはN88-BASICには存在しないので0固定
    params.opna.lfoSyncDelay = (int)pN88LfoSyncDelay->load(std::memory_order_relaxed);
    params.opna.pan = (int)pPan->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.opna.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.opna.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.opna.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int op = 0; op < OpnaPrValue::ops; ++op)
    {
        params.opna.op[op].multiple = (int)pOpMultiple[op]->load(std::memory_order_relaxed);
        params.opna.op[op].detune = (int)pOpDetune[op]->load(std::memory_order_relaxed);

        params.opna.op[op].ssgEg = (int)pOpSsgEg[op]->load(std::memory_order_relaxed);
        params.opna.op[op].fmSsgEgFreq = pOpSsgEgFreq[op]->load(std::memory_order_relaxed);

        params.opna.op[op].fixedMode = (pOpFixEnable[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
        params.opna.op[op].fixedFreq = pOpFixFreq[op]->load(std::memory_order_relaxed);

        params.opna.op[op].waveSelect = 0; // Sine
        params.opna.op[op].lfoFreqIndex = (int)pOpLfoFreqs[op]->load(std::memory_order_relaxed);
        params.opna.op[op].lfoSyncDelay = (int)pOpLfoSyncDelay[op]->load(std::memory_order_relaxed);
        params.opna.op[op].vibEnable = (pOpLfoPmEnable[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
        params.opna.op[op].pms = (int)pOpLfoPms[op]->load(std::memory_order_relaxed);
        params.opna.op[op].amEnable = (pOpLfoAmEnable[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
        params.opna.op[op].ams = (int)pOpLfoAms[op]->load(std::memory_order_relaxed);
        params.opna.op[op].amd = 0; // ローカルAMDは未使用なので0固定
        params.opna.op[op].egType = true;

        params.opna.op[op].n88Ams = (int)pOpN88LfoAms[op]->load(std::memory_order_relaxed);

        params.opna.op[op].mask = (pOpMask[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);

        params.opna.op[op].m_adsrParams.ar = (int)pOpAdsrRgAr[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.dr = (int)pOpAdsrRgD1r[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.sl = (int)pOpAdsrRgD1l[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.sr = (int)pOpAdsrRgD2r[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.rr = (int)pOpAdsrRgRr[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.tl = (int)pOpAdsrRgTl[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.ks = (int)pOpAdsrKs[op]->load(std::memory_order_relaxed);
        params.opna.op[op].m_adsrParams.xof = (pOpAdsrXof[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
        params.opna.op[op].m_adsrParams.kor = pOpAdsrKor[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread;
        params.opna.op[op].m_adsrParams.bypass = pOpAdsrBypass[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread;

        params.opna.op[op].pitchEnvEnable = pOpPitchEnvEnable[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread;
        params.opna.op[op].pitchAdsr.bypass = false;
        params.opna.op[op].pitchAdsr.ar = pOpPitchEnvAr[op]->load(std::memory_order_relaxed);
        params.opna.op[op].pitchAdsr.dr = pOpPitchEnvDr[op]->load(std::memory_order_relaxed);
        params.opna.op[op].pitchAdsr.rr = pOpPitchEnvRr[op]->load(std::memory_order_relaxed);
        params.opna.op[op].pitchAdsr.stl = (int)pOpPitchEnvStl[op]->load(std::memory_order_relaxed);
        params.opna.op[op].pitchAdsr.atl = (int)pOpPitchEnvAtl[op]->load(std::memory_order_relaxed);
        params.opna.op[op].pitchAdsr.ssl = (int)pOpPitchEnvSsl[op]->load(std::memory_order_relaxed);
        params.opna.op[op].pitchAdsr.rll = (int)pOpPitchEnvRll[op]->load(std::memory_order_relaxed);

        params.opna.op[op].ssgEnvEnable = (pOpSsgSwEnvEnable[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
        params.opna.op[op].ssgSwEnv.bypass = false;
        params.opna.op[op].ssgSwEnv.steps = (int)pOpSsgSwEnvSteps[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.loop = (pOpSsgSwEnvLoop[op]->load(std::memory_order_relaxed) > OpnaPrValue::boolThread);
        params.opna.op[op].ssgSwEnv.loopTo = (int)pOpSsgSwEnvLoopTo[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.loopCount = (int)pOpSsgSwEnvLoopCount[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.stl = pOpSsgSwEnvStl[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.r1 = pOpSsgSwEnvR1[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.l1 = pOpSsgSwEnvL1[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.r2 = pOpSsgSwEnvR2[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.l2 = pOpSsgSwEnvL2[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.r3 = pOpSsgSwEnvR3[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.l3 = pOpSsgSwEnvL3[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.r4 = pOpSsgSwEnvR4[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.l4 = pOpSsgSwEnvL4[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.r5 = pOpSsgSwEnvR5[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.l5 = pOpSsgSwEnvL5[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.r6 = pOpSsgSwEnvR6[op]->load(std::memory_order_relaxed);
        params.opna.op[op].ssgSwEnv.l6 = pOpSsgSwEnvL6[op]->load(std::memory_order_relaxed);
    }
}
