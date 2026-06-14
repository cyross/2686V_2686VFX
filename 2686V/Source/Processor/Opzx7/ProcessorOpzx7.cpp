#include "./ProcessorOpzx7.h"

#include "./ProcessorOpzx7Keys.h"
#include "./ProcessorOpzx7Values.h"
#include "./ProcessorOpzx7Names.h"

void Opzx7Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = Opzx7PrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::level, code + Opzx7PrName::level, Opzx7PrValue::Level::min, Opzx7PrValue::Level::max, Opzx7PrValue::Level::initial));

    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::alg, code + Opzx7PrName::alg, Opzx7PrValue::Alg::min, Opzx7PrValue::Alg::max, Opzx7PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::fb, code + Opzx7PrName::fb, Opzx7PrValue::Fb::min, Opzx7PrValue::Fb::max, Opzx7PrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::bit, code + Opzx7PrName::bit, Opzx7PrValue::Bit::min, Opzx7PrValue::Bit::max, Opzx7PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::rate, code + Opzx7PrName::rate, Opzx7PrValue::Rate::min, Opzx7PrValue::Rate::max, Opzx7PrValue::Rate::initial)); // Default 6 (16kHz)

    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::panpot, code + Opzx7PrName::panpot, Opzx7PrValue::Panpot::min, Opzx7PrValue::Panpot::max, Opzx7PrValue::Panpot::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Opzx7PrKey::panpot_en, code + Opzx7PrName::panpot_en, Opzx7PrValue::PanpotEnable::initial)); // PM Enable (Switch)

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::Unison::voices, code + Opzx7PrName::Unison::voices, Opzx7PrValue::Unison::Voices::min, Opzx7PrValue::Unison::Voices::max, Opzx7PrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Opzx7PrKey::Unison::detune, code + Opzx7PrName::Unison::detune, Opzx7PrValue::Unison::Detune::min, Opzx7PrValue::Unison::Detune::max, Opzx7PrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Opzx7PrKey::Unison::spread, code + Opzx7PrName::Unison::spread, Opzx7PrValue::Unison::Spread::min, Opzx7PrValue::Unison::Spread::max, Opzx7PrValue::Unison::Spread::initial));

    addOpzx7LfoParameters(layout, code);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String prefix = code + Opzx7PrKey::op + juce::String(op);
        juce::String namePrefix = code + Opzx7PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::mul, namePrefix + Opzx7PrName::mul, Opzx7PrValue::Op::Mul::min, Opzx7PrValue::Op::Mul::max, Opzx7PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::mulRatio, namePrefix + Opzx7PrName::mulRatio, Opzx7PrValue::Op::MulRatio::min, Opzx7PrValue::Op::MulRatio::max, Opzx7PrValue::Op::MulRatio::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::dt, namePrefix + Opzx7PrName::dt1, Opzx7PrValue::Op::Dt1::min, Opzx7PrValue::Op::Dt1::max, Opzx7PrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::dt2, namePrefix + Opzx7PrName::dt2, Opzx7PrValue::Op::Dt2::min, Opzx7PrValue::Op::Dt2::max, Opzx7PrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::dt3, namePrefix + Opzx7PrName::dt3, Opzx7PrValue::Op::Dt3::min, Opzx7PrValue::Op::Dt3::max, Opzx7PrValue::Op::Dt3::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::tl, namePrefix + Opzx7PrName::tl, Opzx7PrValue::Op::Adsr::Tl::min, Opzx7PrValue::Op::Adsr::Tl::max, Opzx7PrValue::Op::Adsr::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::ar, namePrefix + Opzx7PrName::ar, Opzx7PrValue::Op::Adsr::Ar::min, Opzx7PrValue::Op::Adsr::Ar::max, Opzx7PrValue::Op::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::d1r, namePrefix + Opzx7PrName::d1r, Opzx7PrValue::Op::Adsr::D1r::min, Opzx7PrValue::Op::Adsr::D1r::max, Opzx7PrValue::Op::Adsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::d1l, namePrefix + Opzx7PrName::d1l, Opzx7PrValue::Op::Adsr::D1l::min, Opzx7PrValue::Op::Adsr::D1l::max, Opzx7PrValue::Op::Adsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::d2r, namePrefix + Opzx7PrName::d2r, Opzx7PrValue::Op::Adsr::D2r::min, Opzx7PrValue::Op::Adsr::D2r::max, Opzx7PrValue::Op::Adsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::rr, namePrefix + Opzx7PrName::rr, Opzx7PrValue::Op::Adsr::Rr::min, Opzx7PrValue::Op::Adsr::Rr::max, Opzx7PrValue::Op::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::se, namePrefix + Opzx7PrName::se, Opzx7PrValue::Op::Se::min, Opzx7PrValue::Op::Se::max, Opzx7PrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::seFreq, namePrefix + Opzx7PrName::seFreq, Opzx7PrValue::Op::SeFreq::min, Opzx7PrValue::Op::SeFreq::max, Opzx7PrValue::Op::SeFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::ksr, namePrefix + Opzx7PrName::ksr, Opzx7PrValue::Op::Ksr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::ksl, namePrefix + Opzx7PrName::ksl, Opzx7PrValue::Op::Ksl::min, Opzx7PrValue::Op::Ksl::max, Opzx7PrValue::Op::Ksl::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::ksEn, namePrefix + Opzx7PrName::ksEn, Opzx7PrValue::Op::KsEn::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::PitchAdsr::enable, namePrefix + Opzx7PrName::PitchAdsr::enable, Opzx7PrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::SsgSwEnv::enable, namePrefix + Opzx7PrName::SsgSwEnv::enable, Opzx7PrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::fix, namePrefix + Opzx7PrName::fix, Opzx7PrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::fixFreq, namePrefix + Opzx7PrName::fixFreq, Opzx7PrValue::Op::FixFreq::min, Opzx7PrValue::Op::FixFreq::max, Opzx7PrValue::Op::FixFreq::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::ws, namePrefix + Opzx7PrName::ws, Opzx7PrValue::Op::Ws::min, Opzx7PrValue::Op::Ws::max, Opzx7PrValue::Op::Ws::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::pcmOffset, namePrefix + Opzx7PrName::pcmOffset, Opzx7PrValue::Op::Offset::min, Opzx7PrValue::Op::Offset::max, Opzx7PrValue::Op::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::pcmRatio, namePrefix + Opzx7PrName::pcmRatio, Opzx7PrValue::Op::Ratio::min, Opzx7PrValue::Op::Ratio::max, Opzx7PrValue::Op::Ratio::initial));


        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::mask, namePrefix + Opzx7PrName::mask, Opzx7PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::rgEn, namePrefix + Opzx7PrName::rgEn, Opzx7PrValue::Op::RgAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgAr, namePrefix + Opzx7PrName::rgAr, Opzx7PrValue::Op::RgAdsr::Ar::min, Opzx7PrValue::Op::RgAdsr::Ar::max, Opzx7PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgD1r, namePrefix + Opzx7PrName::rgD1r, Opzx7PrValue::Op::RgAdsr::D1r::min, Opzx7PrValue::Op::RgAdsr::D1r::max, Opzx7PrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgD1l, namePrefix + Opzx7PrName::rgD1l, Opzx7PrValue::Op::RgAdsr::D1l::min, Opzx7PrValue::Op::RgAdsr::D1l::max, Opzx7PrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgD2r, namePrefix + Opzx7PrName::rgD2r, Opzx7PrValue::Op::RgAdsr::D2r::min, Opzx7PrValue::Op::RgAdsr::D2r::max, Opzx7PrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgRr, namePrefix + Opzx7PrName::rgRr, Opzx7PrValue::Op::RgAdsr::Rr::min, Opzx7PrValue::Op::RgAdsr::Rr::max, Opzx7PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rgTl, namePrefix + Opzx7PrName::rgTl, Opzx7PrValue::Op::RgAdsr::Tl::min, Opzx7PrValue::Op::RgAdsr::Tl::max, Opzx7PrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::sus, namePrefix + Opzx7PrName::sus, Opzx7PrValue::Op::Sus::initial)); // Sus (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::xof, namePrefix + Opzx7PrName::xof, Opzx7PrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::kor, namePrefix + Opzx7PrName::kor, Opzx7PrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::ampBypass, namePrefix + Opzx7PrName::ampBypass, Opzx7PrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, prefix, namePrefix);
        addOpSsgSwEnvParameters(layout, prefix, namePrefix);
        addOpOpzx7LfoParameters(layout, prefix, namePrefix);
    }
}

void Opzx7Processor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = Opzx7PrKey::prefix;

    pLevel = apvts.getRawParameterValue(code + Opzx7PrKey::level);

    pAlg = apvts.getRawParameterValue(code + Opzx7PrKey::alg);
    pFb = apvts.getRawParameterValue(code + Opzx7PrKey::fb);
    pDepth = apvts.getRawParameterValue(code + Opzx7PrKey::bit);
    pRate = apvts.getRawParameterValue(code + Opzx7PrKey::rate);

    pLfoPmSyncDelay = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pmSyncDelay);
    pLfoAmSyncDelay = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amSyncDelay);
    pLfoAmSmoothRatio = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amSmoothRatio);
    pLfoPmFreq = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pmFreq);
    pLfoAmFreq = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amFreq);
    pLfoPmShape = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pgShape);
    pLfoAmShape = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::egShape);
    pLfoPm = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pm);
    pLfoAm = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::am);
    pLfoPmd = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pmd);
    pLfoPms = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pms);
    pLfoAmd = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amd);
    pLfoAms = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::ams);

    pPanpotEnable = apvts.getRawParameterValue(code + Opzx7PrKey::panpot_en);
    pPanpot = apvts.getRawParameterValue(code + Opzx7PrKey::panpot);

	pUnisonVoices = apvts.getRawParameterValue(code + Opzx7PrKey::Unison::voices);
	pUnisonDetuneCents = apvts.getRawParameterValue(code + Opzx7PrKey::Unison::detune);
	pUnisonSpread = apvts.getRawParameterValue(code + Opzx7PrKey::Unison::spread);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String p = code + Opzx7PrKey::op + juce::String(op);

		pOpMultiple[op] = apvts.getRawParameterValue(p + Opzx7PrKey::mul);
		pOpMultipleRatio[op] = apvts.getRawParameterValue(p + Opzx7PrKey::mulRatio);
		pOpDetune[op] = apvts.getRawParameterValue(p + Opzx7PrKey::dt);
		pOpDetune2[op] = apvts.getRawParameterValue(p + Opzx7PrKey::dt2);
        pOpDetune3[op] = apvts.getRawParameterValue(p + Opzx7PrKey::dt3);

        pOpAdsrBypass[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ampBypass);
        pOpAdsrRgEnable[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgEn);
		pOpAdsrRealAr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ar);
		pOpAdsrRealD1r[op] = apvts.getRawParameterValue(p + Opzx7PrKey::d1r);
		pOpAdsrRealD1l[op] = apvts.getRawParameterValue(p + Opzx7PrKey::d1l);
		pOpAdsrRealD2r[op] = apvts.getRawParameterValue(p + Opzx7PrKey::d2r);
		pOpAdsrRealRr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rr);
		pOpAdsrRealTl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::tl);
		pOpAdsrRgAr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgAr);
		pOpAdsrRgD1r[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgD1r);
		pOpAdsrRgD1l[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgD1l);
		pOpAdsrRgD2r[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgD2r);
		pOpAdsrRgRr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgRr);
		pOpAdsrRgTl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::rgTl);
		pOpAdsrKsr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksr);
        pOpAdsrKsl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksl);
        pOpAdsrKsEn[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksEn);
        pOpAdsrSus[op] = apvts.getRawParameterValue(p + Opzx7PrKey::sus);
		pOpAdsrXof[op] = apvts.getRawParameterValue(p + Opzx7PrKey::xof);
        pOpAdsrKor[op] = apvts.getRawParameterValue(p + Opzx7PrKey::kor);

		pOpSsgEg[op] = apvts.getRawParameterValue(p + Opzx7PrKey::se);
		pOpSsgEgFreq[op] = apvts.getRawParameterValue(p + Opzx7PrKey::seFreq);

		pOpFixEnable[op] = apvts.getRawParameterValue(p + Opzx7PrKey::fix);
		pOpFixFreq[op] = apvts.getRawParameterValue(p + Opzx7PrKey::fixFreq);

		pOpWaveSelect[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ws);
		pOpPcmOffset[op] = apvts.getRawParameterValue(p + Opzx7PrKey::pcmOffset);
		pOpPcmRatio[op] = apvts.getRawParameterValue(p + Opzx7PrKey::pcmRatio);

        pOpLfoPmSyncDelay[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::pmSyncDelay);
        pOpLfoAmSyncDelay[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::amSyncDelay);
        pOpLfoAmSmoothRatio[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::amSmoothRatio);
        pOpLfoPmFreq[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::pmFreq);
        pOpLfoAmFreq[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::amFreq);
        pOpLfoPmShape[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::pgShape);
        pOpLfoAmShape[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::egShape);
        pOpLfoPm[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::pm);
        pOpLfoAm[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::am);
        pOpLfoPmd[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::pmd);
        pOpLfoPms[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::pms);
        pOpLfoAmd[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::amd);
        pOpLfoAms[op] = apvts.getRawParameterValue(p + CorePrKey::Post::Lfo::ams);

		pOpPitchEnvEnable[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::enable);
		pOpPitchEnvAr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::ar);
		pOpPitchEnvDr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::dr);
		pOpPitchEnvRr[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::rr);
		pOpPitchEnvStl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::stl);
		pOpPitchEnvAtl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::atl);
		pOpPitchEnvSsl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::ssl);
		pOpPitchEnvRll[op] = apvts.getRawParameterValue(p + Opzx7PrKey::PitchAdsr::rll);

		pOpSsgSwEnvEnable[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::enable);
		pOpSsgSwEnvSteps[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::steps);
		pOpSsgSwEnvLoop[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::loop);
		pOpSsgSwEnvLoopTo[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::loopTo);
		pOpSsgSwEnvLoopCount[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::loopCount);
		pOpSsgSwEnvStl[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::stl);
		pOpSsgSwEnvR1[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::r1);
		pOpSsgSwEnvL1[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::l1);
		pOpSsgSwEnvR2[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::r2);
		pOpSsgSwEnvL2[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::l2);
		pOpSsgSwEnvR3[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::r3);
		pOpSsgSwEnvL3[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::l3);
		pOpSsgSwEnvR4[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::r4);
		pOpSsgSwEnvL4[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::l4);
		pOpSsgSwEnvR5[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::r5);
		pOpSsgSwEnvL5[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::l5);
		pOpSsgSwEnvR6[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::r6);
		pOpSsgSwEnvL6[op] = apvts.getRawParameterValue(p + Opzx7PrKey::SsgSwEnv::l6);

		pOpMask[op] = apvts.getRawParameterValue(p + Opzx7PrKey::mask);
    }
}

void Opzx7Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.opzx7.level = pLevel->load(std::memory_order_relaxed);

    params.opzx7.algorithm = (int)pAlg->load(std::memory_order_relaxed);

    params.opzx7.feedback = pFb->load(std::memory_order_relaxed);

    params.opzx7.fmBitDepth = (int)pDepth->load(std::memory_order_relaxed);
    params.opzx7.fmRateIndex = (int)pRate->load(std::memory_order_relaxed);

    params.opzx7.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.opzx7.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.opzx7.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.opzx7.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
    params.opzx7.pmEnable = (pLfoPm->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
    params.opzx7.pgLfoWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.opzx7.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.opzx7.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.opzx7.amEnable = (pLfoAm->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
    params.opzx7.egLfoWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.opzx7.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.opzx7.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.opzx7.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);

    params.opzx7.panpot_enable = (pPanpotEnable->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
    params.opzx7.panpot = pPanpot->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.opzx7.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.opzx7.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.opzx7.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        params.opzx7.op[op].multiple = (int)pOpMultiple[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].multipleRatio = pOpMultipleRatio[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].detune = (int)pOpDetune[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].detune2 = (int)pOpDetune2[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].detune3 = (int)pOpDetune3[op]->load(std::memory_order_relaxed);

        params.opzx7.op[op].m_adsrParams.rgEnable = pOpAdsrRgEnable[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread;

        params.opzx7.op[op].m_adsrParams.real.ar = pOpAdsrRealAr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.real.d1r = pOpAdsrRealD1r[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.real.d1l = pOpAdsrRealD1l[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.real.d2r = pOpAdsrRealD2r[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.real.rr = pOpAdsrRealRr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.real.tl = pOpAdsrRealTl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.rg.ar = (int)pOpAdsrRgAr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.rg.d1r = (int)pOpAdsrRgD1r[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.rg.d1l = (int)pOpAdsrRgD1l[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.rg.d2r = (int)pOpAdsrRgD2r[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.rg.rr = (int)pOpAdsrRgRr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.rg.tl = (int)pOpAdsrRgTl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksr = (pOpAdsrKsr[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].m_adsrParams.ksl = (int)pOpAdsrKsl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksEn = (pOpAdsrKsEn[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].m_adsrParams.sus = (pOpAdsrSus[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].m_adsrParams.xof = (pOpAdsrXof[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].m_adsrParams.kor = pOpAdsrKor[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread;
        params.opzx7.op[op].m_adsrParams.bypass = pOpAdsrBypass[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread;

        params.opzx7.op[op].ssgEg = (int)pOpSsgEg[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].fmSsgEgFreq = pOpSsgEgFreq[op]->load(std::memory_order_relaxed);

        params.opzx7.op[op].fixedMode = (pOpFixEnable[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].fixedFreq = pOpFixFreq[op]->load(std::memory_order_relaxed);

        params.opzx7.op[op].waveSelect = (int)pOpWaveSelect[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pcmOffset = pOpPcmOffset[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pcmRatio = pOpPcmRatio[op]->load(std::memory_order_relaxed);

        params.opzx7.op[op].lfoPmFreq = pOpLfoPmFreq[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].lfoAmFreq = pOpLfoAmFreq[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].lfoPmSyncDelay = (int)pOpLfoPmSyncDelay[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].lfoAmSyncDelay = (int)pOpLfoAmSyncDelay[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].vibEnable = (pOpLfoPm[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].amEnable = (pOpLfoAm[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].pgLfoWave = (int)pOpLfoPmShape[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].egLfoWave = (int)pOpLfoAmShape[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pms = pOpLfoPms[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ams = pOpLfoAms[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pmd = pOpLfoPmd[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].amd = pOpLfoAmd[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].lfoAmSmRt = pOpLfoAmSmoothRatio[op]->load(std::memory_order_relaxed);

		params.opzx7.op[op].pitchEnvEnable = pOpPitchEnvEnable[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread;
        params.opzx7.op[op].pitchAdsr.bypass = false;
        params.opzx7.op[op].pitchAdsr.ar = pOpPitchEnvAr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pitchAdsr.dr = pOpPitchEnvDr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pitchAdsr.rr = pOpPitchEnvRr[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pitchAdsr.stl = (int)pOpPitchEnvStl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pitchAdsr.atl = (int)pOpPitchEnvAtl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pitchAdsr.ssl = (int)pOpPitchEnvSsl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].pitchAdsr.rll = (int)pOpPitchEnvRll[op]->load(std::memory_order_relaxed);

        params.opzx7.op[op].ssgEnvEnable = (pOpSsgSwEnvEnable[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].ssgSwEnv.bypass = false;
        params.opzx7.op[op].ssgSwEnv.steps = (int)pOpSsgSwEnvSteps[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.loop = (pOpSsgSwEnvLoop[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].ssgSwEnv.loopTo = (int)pOpSsgSwEnvLoopTo[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.loopCount = (int)pOpSsgSwEnvLoopCount[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.stl = pOpSsgSwEnvStl[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.r1 = pOpSsgSwEnvR1[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.l1 = pOpSsgSwEnvL1[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.r2 = pOpSsgSwEnvR2[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.l2 = pOpSsgSwEnvL2[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.r3 = pOpSsgSwEnvR3[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.l3 = pOpSsgSwEnvL3[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.r4 = pOpSsgSwEnvR4[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.l4 = pOpSsgSwEnvL4[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.r5 = pOpSsgSwEnvR5[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.l5 = pOpSsgSwEnvL5[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.r6 = pOpSsgSwEnvR6[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].ssgSwEnv.l6 = pOpSsgSwEnvL6[op]->load(std::memory_order_relaxed);

        params.opzx7.op[op].mask = pOpMask[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread;
    }
}
