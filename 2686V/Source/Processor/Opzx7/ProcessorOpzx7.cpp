#include "./ProcessorOpzx7.h"

#include "./ProcessorOpzx7Keys.h"
#include "./ProcessorOpzx7Values.h"
#include "./ProcessorOpzx7Names.h"

void Opzx7Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = Opzx7PrKey::prefix;
    const juce::String prefixName = Opzx7PrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::level, prefixName + Opzx7PrName::level, Opzx7PrValue::Level::min, Opzx7PrValue::Level::max, Opzx7PrValue::Level::initial));

    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::alg, prefixName + Opzx7PrName::alg, Opzx7PrValue::Alg::min, Opzx7PrValue::Alg::max, Opzx7PrValue::Alg::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::fb, prefixName + Opzx7PrName::fb, Opzx7PrValue::Fb::min, Opzx7PrValue::Fb::max, Opzx7PrValue::Fb::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::bit, prefixName + Opzx7PrName::bit, Opzx7PrValue::Bit::min, Opzx7PrValue::Bit::max, Opzx7PrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::rate, prefixName + Opzx7PrName::rate, Opzx7PrValue::Rate::min, Opzx7PrValue::Rate::max, Opzx7PrValue::Rate::initial)); // Default 6 (16kHz)

    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::panpot, prefixName + Opzx7PrName::panpot, Opzx7PrValue::Panpot::min, Opzx7PrValue::Panpot::max, Opzx7PrValue::Panpot::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + Opzx7PrKey::panpot_en, prefixName + Opzx7PrName::panpot_en, Opzx7PrValue::PanpotEnable::initial)); // PM Enable (Switch)

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::Unison::voices, prefixName + Opzx7PrName::Unison::voices, Opzx7PrValue::Unison::Voices::min, Opzx7PrValue::Unison::Voices::max, Opzx7PrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + Opzx7PrKey::Unison::detune, prefixName + Opzx7PrName::Unison::detune, Opzx7PrValue::Unison::Detune::min, Opzx7PrValue::Unison::Detune::max, Opzx7PrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + Opzx7PrKey::Unison::spread, prefixName + Opzx7PrName::Unison::spread, Opzx7PrValue::Unison::Spread::min, Opzx7PrValue::Unison::Spread::max, Opzx7PrValue::Unison::Spread::initial));

    addOpzx7LfoParameters(layout, prefix, prefixName);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String opPrefix = prefix + Opzx7PrKey::op + juce::String(op);
        juce::String opPrefixName = prefixName + Opzx7PrName::op + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::mul, opPrefixName + Opzx7PrName::mul, Opzx7PrValue::Op::Mul::min, Opzx7PrValue::Op::Mul::max, Opzx7PrValue::Op::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::mulRatio, opPrefixName + Opzx7PrName::mulRatio, Opzx7PrValue::Op::MulRatio::min, Opzx7PrValue::Op::MulRatio::max, Opzx7PrValue::Op::MulRatio::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::dt, opPrefixName + Opzx7PrName::dt1, Opzx7PrValue::Op::Dt1::min, Opzx7PrValue::Op::Dt1::max, Opzx7PrValue::Op::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::dt2, opPrefixName + Opzx7PrName::dt2, Opzx7PrValue::Op::Dt2::min, Opzx7PrValue::Op::Dt2::max, Opzx7PrValue::Op::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::dt3, opPrefixName + Opzx7PrName::dt3, Opzx7PrValue::Op::Dt3::min, Opzx7PrValue::Op::Dt3::max, Opzx7PrValue::Op::Dt3::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::tl, opPrefixName + Opzx7PrName::tl, Opzx7PrValue::Op::Adsr::Tl::min, Opzx7PrValue::Op::Adsr::Tl::max, Opzx7PrValue::Op::Adsr::Tl::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::ar, opPrefixName + Opzx7PrName::ar, Opzx7PrValue::Op::Adsr::Ar::min, Opzx7PrValue::Op::Adsr::Ar::max, Opzx7PrValue::Op::Adsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::d1r, opPrefixName + Opzx7PrName::d1r, Opzx7PrValue::Op::Adsr::D1r::min, Opzx7PrValue::Op::Adsr::D1r::max, Opzx7PrValue::Op::Adsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::d1l, opPrefixName + Opzx7PrName::d1l, Opzx7PrValue::Op::Adsr::D1l::min, Opzx7PrValue::Op::Adsr::D1l::max, Opzx7PrValue::Op::Adsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::d2r, opPrefixName + Opzx7PrName::d2r, Opzx7PrValue::Op::Adsr::D2r::min, Opzx7PrValue::Op::Adsr::D2r::max, Opzx7PrValue::Op::Adsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::rr, opPrefixName + Opzx7PrName::rr, Opzx7PrValue::Op::Adsr::Rr::min, Opzx7PrValue::Op::Adsr::Rr::max, Opzx7PrValue::Op::Adsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::se, opPrefixName + Opzx7PrName::se, Opzx7PrValue::Op::Se::min, Opzx7PrValue::Op::Se::max, Opzx7PrValue::Op::Se::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::seFreq, opPrefixName + Opzx7PrName::seFreq, Opzx7PrValue::Op::SeFreq::min, Opzx7PrValue::Op::SeFreq::max, Opzx7PrValue::Op::SeFreq::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::ksEn, opPrefixName + Opzx7PrName::ksEn, Opzx7PrValue::Op::KsEn::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ksMode, opPrefixName + Opzx7PrName::ksMode, Opzx7PrValue::Op::KsMode::min, Opzx7PrValue::Op::KsMode::max, Opzx7PrValue::Op::KsMode::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::ksrMA7, opPrefixName + Opzx7PrName::ksrMA7, Opzx7PrValue::Op::KsMA7::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::kslMA7, opPrefixName + Opzx7PrName::kslMA7, Opzx7PrValue::Op::KslMA7::min, Opzx7PrValue::Op::KslMA7::max, Opzx7PrValue::Op::KslMA7::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ksrOPZ, opPrefixName + Opzx7PrName::ksrOPZ, Opzx7PrValue::Op::KsrOPZ::min, Opzx7PrValue::Op::KsrOPZ::max, Opzx7PrValue::Op::KsrOPZ::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::kslOPZ, opPrefixName + Opzx7PrName::kslOPZ, Opzx7PrValue::Op::KslOPZ::min, Opzx7PrValue::Op::KslOPZ::max, Opzx7PrValue::Op::KslOPZ::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ksBp, opPrefixName + Opzx7PrName::ksBp, Opzx7PrValue::Op::KsBp::min, Opzx7PrValue::Op::KsBp::max, Opzx7PrValue::Op::KsBp::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ksLc, opPrefixName + Opzx7PrName::ksLc, Opzx7PrValue::Op::KsLc::min, Opzx7PrValue::Op::KsLc::max, Opzx7PrValue::Op::KsLc::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ksRc, opPrefixName + Opzx7PrName::ksRc, Opzx7PrValue::Op::KsRc::min, Opzx7PrValue::Op::KsRc::max, Opzx7PrValue::Op::KsRc::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::ksLd, opPrefixName + Opzx7PrName::ksLd, Opzx7PrValue::Op::KsLd::min, Opzx7PrValue::Op::KsLd::max, Opzx7PrValue::Op::KsLd::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::ksRd, opPrefixName + Opzx7PrName::ksRd, Opzx7PrValue::Op::KsRd::min, Opzx7PrValue::Op::KsRd::max, Opzx7PrValue::Op::KsRd::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ksRs, opPrefixName + Opzx7PrName::ksRs, Opzx7PrValue::Op::KsRs::min, Opzx7PrValue::Op::KsRs::max, Opzx7PrValue::Op::KsRs::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::PitchAdsr::enable, opPrefixName + Opzx7PrName::PitchAdsr::enable, Opzx7PrValue::Op::PitchAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::SsgSwEnv::enable, opPrefixName + Opzx7PrName::SsgSwEnv::enable, Opzx7PrValue::Op::SsgSwEnv::Enable::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::fix, opPrefixName + Opzx7PrName::fix, Opzx7PrValue::Op::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::fixFreq, opPrefixName + Opzx7PrName::fixFreq, Opzx7PrValue::Op::FixFreq::min, Opzx7PrValue::Op::FixFreq::max, Opzx7PrValue::Op::FixFreq::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::ws, opPrefixName + Opzx7PrName::ws, Opzx7PrValue::Op::Ws::min, Opzx7PrValue::Op::Ws::max, Opzx7PrValue::Op::Ws::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::pcmOffset, opPrefixName + Opzx7PrName::pcmOffset, Opzx7PrValue::Op::Offset::min, Opzx7PrValue::Op::Offset::max, Opzx7PrValue::Op::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(opPrefix + Opzx7PrKey::pcmRatio, opPrefixName + Opzx7PrName::pcmRatio, Opzx7PrValue::Op::Ratio::min, Opzx7PrValue::Op::Ratio::max, Opzx7PrValue::Op::Ratio::initial));


        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::mask, opPrefixName + Opzx7PrName::mask, Opzx7PrValue::Op::Mask::initial)); // OP Mask (Switch)

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::rgEn, opPrefixName + Opzx7PrName::rgEn, Opzx7PrValue::Op::RgAdsr::Enable::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::rgAr, opPrefixName + Opzx7PrName::rgAr, Opzx7PrValue::Op::RgAdsr::Ar::min, Opzx7PrValue::Op::RgAdsr::Ar::max, Opzx7PrValue::Op::RgAdsr::Ar::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::rgD1r, opPrefixName + Opzx7PrName::rgD1r, Opzx7PrValue::Op::RgAdsr::D1r::min, Opzx7PrValue::Op::RgAdsr::D1r::max, Opzx7PrValue::Op::RgAdsr::D1r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::rgD1l, opPrefixName + Opzx7PrName::rgD1l, Opzx7PrValue::Op::RgAdsr::D1l::min, Opzx7PrValue::Op::RgAdsr::D1l::max, Opzx7PrValue::Op::RgAdsr::D1l::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::rgD2r, opPrefixName + Opzx7PrName::rgD2r, Opzx7PrValue::Op::RgAdsr::D2r::min, Opzx7PrValue::Op::RgAdsr::D2r::max, Opzx7PrValue::Op::RgAdsr::D2r::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::rgRr, opPrefixName + Opzx7PrName::rgRr, Opzx7PrValue::Op::RgAdsr::Rr::min, Opzx7PrValue::Op::RgAdsr::Rr::max, Opzx7PrValue::Op::RgAdsr::Rr::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(opPrefix + Opzx7PrKey::rgTl, opPrefixName + Opzx7PrName::rgTl, Opzx7PrValue::Op::RgAdsr::Tl::min, Opzx7PrValue::Op::RgAdsr::Tl::max, Opzx7PrValue::Op::RgAdsr::Tl::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::sus, opPrefixName + Opzx7PrName::sus, Opzx7PrValue::Op::Sus::initial)); // Sus (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::xof, opPrefixName + Opzx7PrName::xof, Opzx7PrValue::Op::Xof::initial)); // Xof (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::kor, opPrefixName + Opzx7PrName::kor, Opzx7PrValue::Op::Kor::initial)); // Kor (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(opPrefix + Opzx7PrKey::ampBypass, opPrefixName + Opzx7PrName::ampBypass, Opzx7PrValue::Op::AmpBypass::initial)); // Bypass (Switch)

        addOpPitchEnvParameters(layout, opPrefix, opPrefixName);
        addOpSsgSwEnvParameters(layout, opPrefix, opPrefixName);
        addOpOpzx7LfoParameters(layout, opPrefix, opPrefixName);
    }
}

void Opzx7Processor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = Opzx7PrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + Opzx7PrKey::level);

    pAlg = apvts.getRawParameterValue(prefix + Opzx7PrKey::alg);
    pFb = apvts.getRawParameterValue(prefix + Opzx7PrKey::fb);
    pDepth = apvts.getRawParameterValue(prefix + Opzx7PrKey::bit);
    pRate = apvts.getRawParameterValue(prefix + Opzx7PrKey::rate);

    pLfoPmSyncDelay = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pmSyncDelay);
    pLfoAmSyncDelay = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amSyncDelay);
    pLfoAmSmoothRatio = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amSmoothRatio);
    pLfoPmFreq = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pmFreq);
    pLfoAmFreq = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amFreq);
    pLfoPmShape = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pgShape);
    pLfoAmShape = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::egShape);
    pLfoPm = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pm);
    pLfoAm = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::am);
    pLfoPmd = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pmd);
    pLfoPms = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::pms);
    pLfoAmd = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::amd);
    pLfoAms = apvts.getRawParameterValue(prefix + CorePrKey::Post::Lfo::ams);

    pPanpotEnable = apvts.getRawParameterValue(prefix + Opzx7PrKey::panpot_en);
    pPanpot = apvts.getRawParameterValue(prefix + Opzx7PrKey::panpot);

	pUnisonVoices = apvts.getRawParameterValue(prefix + Opzx7PrKey::Unison::voices);
	pUnisonDetuneCents = apvts.getRawParameterValue(prefix + Opzx7PrKey::Unison::detune);
	pUnisonSpread = apvts.getRawParameterValue(prefix + Opzx7PrKey::Unison::spread);

    for (int op = 0; op < Opzx7PrValue::ops; ++op)
    {
        juce::String p = prefix + Opzx7PrKey::op + juce::String(op);

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
        pOpAdsrKsEn[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksEn);
        pOpAdsrKsMode[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksMode);
        pOpAdsrKsrMA7[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksrMA7);
        pOpAdsrKslMA7[op] = apvts.getRawParameterValue(p + Opzx7PrKey::kslMA7);
        pOpAdsrKsrOPZ[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksrOPZ);
        pOpAdsrKslOPZ[op] = apvts.getRawParameterValue(p + Opzx7PrKey::kslOPZ);
        pOpAdsrKsBp[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksBp);
        pOpAdsrKsLc[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksLc);
        pOpAdsrKsRc[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksRc);
        pOpAdsrKsLd[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksLd);
        pOpAdsrKsRd[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksRd);
        pOpAdsrKsRs[op] = apvts.getRawParameterValue(p + Opzx7PrKey::ksRs);
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

        params.opzx7.op[op].m_adsrParams.ksEn = (pOpAdsrKsEn[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].m_adsrParams.ksMode = (KeyScaleMode)pOpAdsrKsMode[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksrMA7 = (pOpAdsrKsrMA7[op]->load(std::memory_order_relaxed) > Opzx7PrValue::boolThread);
        params.opzx7.op[op].m_adsrParams.kslMA7 = (int)pOpAdsrKslMA7[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksrOPZ = (int)pOpAdsrKsrOPZ[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.kslOPZ = (int)pOpAdsrKslOPZ[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksBp = (int)pOpAdsrKsBp[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksLc = (int)pOpAdsrKsLc[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksRc = (int)pOpAdsrKsRc[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksLd = pOpAdsrKsLd[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksRd = pOpAdsrKsRd[op]->load(std::memory_order_relaxed);
        params.opzx7.op[op].m_adsrParams.ksRs = (int)pOpAdsrKsRs[op]->load(std::memory_order_relaxed);

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
