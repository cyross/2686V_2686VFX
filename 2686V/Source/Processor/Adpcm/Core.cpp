#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = AdpcmPrKey::prefix;

    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::level, code + AdpcmPrName::level, AdpcmPrValue::Level::min, AdpcmPrValue::Level::max, AdpcmPrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::pan, code + AdpcmPrName::pan, AdpcmPrValue::Pan::min, AdpcmPrValue::Pan::max, AdpcmPrValue::Pan::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::loop, code + AdpcmPrName::loop, AdpcmPrValue::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::mode, code + AdpcmPrName::bit, AdpcmPrValue::Bit::min, AdpcmPrValue::Bit::max, AdpcmPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::rate, code + AdpcmPrName::rate, AdpcmPrValue::Rate::min, AdpcmPrValue::Rate::max, AdpcmPrValue::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::pcmOffset, code + AdpcmPrName::pcmOffset, AdpcmPrValue::Offset::min, AdpcmPrValue::Offset::max, AdpcmPrValue::Offset::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::pcmRatio, code + AdpcmPrName::pcmRatio, AdpcmPrValue::Ratio::min, AdpcmPrValue::Ratio::max, AdpcmPrValue::Ratio::initial));

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::fix, code + AdpcmPrName::fix, AdpcmPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::fixFreq, code + AdpcmPrName::fixFreq, AdpcmPrValue::FixFreq::min, AdpcmPrValue::FixFreq::max, AdpcmPrValue::FixFreq::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::adsr + AdpcmPrKey::bypass, code + AdpcmPrName::Adsr::bypass, AdpcmPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass, code + AdpcmPrName::PitchAdsr::bypass, AdpcmPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + AdpcmPrKey::ssgSwEnv + AdpcmPrKey::bypass, code + AdpcmPrName::SsgSwEnv::bypass, AdpcmPrValue::SsgSwEnv::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::mul, code + AdpcmPrName::mul, AdpcmPrValue::Mul::min, AdpcmPrValue::Mul::max, AdpcmPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::mulRatio, code + AdpcmPrName::mulRatio, AdpcmPrValue::MulRatio::min, AdpcmPrValue::MulRatio::max, AdpcmPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::dt, code + AdpcmPrName::dt1, AdpcmPrValue::Dt1::min, AdpcmPrValue::Dt1::max, AdpcmPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::dt2, code + AdpcmPrName::dt2, AdpcmPrValue::Dt2::min, AdpcmPrValue::Dt2::max, AdpcmPrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::dt3, code + AdpcmPrName::dt3, AdpcmPrValue::Dt3::min, AdpcmPrValue::Dt3::max, AdpcmPrValue::Dt3::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::Unison::voices, code + AdpcmPrName::Unison::voices, AdpcmPrValue::Unison::Voices::min, AdpcmPrValue::Unison::Voices::max, AdpcmPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + AdpcmPrKey::Unison::detune, code + AdpcmPrName::Unison::detune, AdpcmPrValue::Unison::Detune::min, AdpcmPrValue::Unison::Detune::max, AdpcmPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + AdpcmPrKey::Unison::spread, code + AdpcmPrName::Unison::spread, AdpcmPrValue::Unison::Spread::min, AdpcmPrValue::Unison::Spread::max, AdpcmPrValue::Unison::Spread::initial));

    addEnvParameters(layout, code);
    addPitchEnvParameters(layout, code);
    addSsgSwEnvParameters(layout, code);
}

void AdpcmProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = AdpcmPrKey::prefix;

    pLevel = apvts.getRawParameterValue(code + AdpcmPrKey::level);
    pPan = apvts.getRawParameterValue(code + AdpcmPrKey::pan);
    pLoop = apvts.getRawParameterValue(code + AdpcmPrKey::loop);
    pQualityMode = apvts.getRawParameterValue(code + AdpcmPrKey::mode);
    pRateIndex = apvts.getRawParameterValue(code + AdpcmPrKey::rate);
    pPcmOffset = apvts.getRawParameterValue(code + AdpcmPrKey::pcmOffset);
    pPcmRatio = apvts.getRawParameterValue(code + AdpcmPrKey::pcmRatio);

    pFixMode = apvts.getRawParameterValue(code + AdpcmPrKey::fix);
    pFixFreq = apvts.getRawParameterValue(code + AdpcmPrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(code + AdpcmPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(code + AdpcmPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(code + AdpcmPrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(code + AdpcmPrKey::adsr + AdpcmPrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(code + AdpcmPrKey::Adsr::rr);

    pPitchAdsrBypass = apvts.getRawParameterValue(code + AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(code + AdpcmPrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(code + AdpcmPrKey::ssgSwEnv + AdpcmPrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(code + AdpcmPrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(code + AdpcmPrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(code + AdpcmPrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(code + AdpcmPrKey::dt);
    pDetune2 = apvts.getRawParameterValue(code + AdpcmPrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(code + AdpcmPrKey::dt3);
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = AdpcmPrKey::prefix;

    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcm.level = pLevel->load(std::memory_order_relaxed);
    params.adpcm.pan = pPan->load(std::memory_order_relaxed);
    params.adpcm.loop = (pLoop->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.qualityMode = (int)pQualityMode->load(std::memory_order_relaxed);
    params.adpcm.rateIndex = (int)pRateIndex->load(std::memory_order_relaxed);
    params.adpcm.offset = pPcmOffset->load(std::memory_order_relaxed);
    params.adpcm.ratio = pPcmRatio->load(std::memory_order_relaxed);

    params.adpcm.fixedMode = (pFixMode->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.fixedFreq = pFixFreq->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.adpcm.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.adpcm.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.adpcm.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    params.adpcm.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
	params.adpcm.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.adpcm.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.adpcm.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.adpcm.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.adpcm.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);

    params.adpcm.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.adpcm.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.adpcm.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.adpcm.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.adpcm.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.adpcm.pitchAdsr.ssl = (int)pPitchAdsrSsl->load(std::memory_order_relaxed);
    params.adpcm.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.adpcm.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.adpcm.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.adpcm.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.adpcm.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.adpcm.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.adpcm.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.adpcm.detune3 = (int)pDetune3->load(std::memory_order_relaxed);
}
