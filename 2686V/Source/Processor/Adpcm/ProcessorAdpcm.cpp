#include "./ProcessorAdpcm.h"

#include "./ProcessorAdpcmKeys.h"
#include "./ProcessorAdpcmValues.h"
#include "./ProcessorAdpcmNames.h"

void AdpcmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = AdpcmPrKey::prefix;
    const juce::String prefixName = AdpcmPrName::prefix;

    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::level, prefixName + AdpcmPrName::level, AdpcmPrValue::Level::min, AdpcmPrValue::Level::max, AdpcmPrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::pan, prefixName + AdpcmPrName::pan, AdpcmPrValue::Pan::min, AdpcmPrValue::Pan::max, AdpcmPrValue::Pan::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + AdpcmPrKey::loop, prefixName + AdpcmPrName::loop, AdpcmPrValue::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::mode, prefixName + AdpcmPrName::bit, AdpcmPrValue::Bit::min, AdpcmPrValue::Bit::max, AdpcmPrValue::Bit::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::rate, prefixName + AdpcmPrName::rate, AdpcmPrValue::Rate::min, AdpcmPrValue::Rate::max, AdpcmPrValue::Rate::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::pcmOffset, prefixName + AdpcmPrName::pcmOffset, AdpcmPrValue::Offset::min, AdpcmPrValue::Offset::max, AdpcmPrValue::Offset::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::pcmRatio, prefixName + AdpcmPrName::pcmRatio, AdpcmPrValue::Ratio::min, AdpcmPrValue::Ratio::max, AdpcmPrValue::Ratio::initial));

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + AdpcmPrKey::fix, prefixName + AdpcmPrName::fix, AdpcmPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::fixFreq, prefixName + AdpcmPrName::fixFreq, AdpcmPrValue::FixFreq::min, AdpcmPrValue::FixFreq::max, AdpcmPrValue::FixFreq::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + AdpcmPrKey::adsr + AdpcmPrKey::bypass, prefixName + AdpcmPrName::Adsr::bypass, AdpcmPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass, prefixName + AdpcmPrName::PitchAdsr::bypass, AdpcmPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + AdpcmPrKey::ssgSwEnv + AdpcmPrKey::bypass, prefixName + AdpcmPrName::SsgSwEnv::bypass, AdpcmPrValue::SsgSwEnv::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::mul, prefixName + AdpcmPrName::mul, AdpcmPrValue::Mul::min, AdpcmPrValue::Mul::max, AdpcmPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::mulRatio, prefixName + AdpcmPrName::mulRatio, AdpcmPrValue::MulRatio::min, AdpcmPrValue::MulRatio::max, AdpcmPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::dt, prefixName + AdpcmPrName::dt1, AdpcmPrValue::Dt1::min, AdpcmPrValue::Dt1::max, AdpcmPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::dt2, prefixName + AdpcmPrName::dt2, AdpcmPrValue::Dt2::min, AdpcmPrValue::Dt2::max, AdpcmPrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::dt3, prefixName + AdpcmPrName::dt3, AdpcmPrValue::Dt3::min, AdpcmPrValue::Dt3::max, AdpcmPrValue::Dt3::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::Unison::voices, prefixName + AdpcmPrName::Unison::voices, AdpcmPrValue::Unison::Voices::min, AdpcmPrValue::Unison::Voices::max, AdpcmPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + AdpcmPrKey::Unison::detune, prefixName + AdpcmPrName::Unison::detune, AdpcmPrValue::Unison::Detune::min, AdpcmPrValue::Unison::Detune::max, AdpcmPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + AdpcmPrKey::Unison::spread, prefixName + AdpcmPrName::Unison::spread, AdpcmPrValue::Unison::Spread::min, AdpcmPrValue::Unison::Spread::max, AdpcmPrValue::Unison::Spread::initial));

    addEnvParameters(layout, prefix, prefixName);
    addPitchEnvParameters(layout, prefix, prefixName);
    addSsgSwEnvParameters(layout, prefix, prefixName);
    addOpzx7LfoParameters(layout, prefix, prefixName);
}

void AdpcmProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = AdpcmPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + AdpcmPrKey::level);
    pPan = apvts.getRawParameterValue(prefix + AdpcmPrKey::pan);
    pLoop = apvts.getRawParameterValue(prefix + AdpcmPrKey::loop);
    pQualityMode = apvts.getRawParameterValue(prefix + AdpcmPrKey::mode);
    pRateIndex = apvts.getRawParameterValue(prefix + AdpcmPrKey::rate);
    pPcmOffset = apvts.getRawParameterValue(prefix + AdpcmPrKey::pcmOffset);
    pPcmRatio = apvts.getRawParameterValue(prefix + AdpcmPrKey::pcmRatio);

    pFixMode = apvts.getRawParameterValue(prefix + AdpcmPrKey::fix);
    pFixFreq = apvts.getRawParameterValue(prefix + AdpcmPrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(prefix + AdpcmPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + AdpcmPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + AdpcmPrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(prefix + AdpcmPrKey::adsr + AdpcmPrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(prefix + AdpcmPrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(prefix + AdpcmPrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(prefix + AdpcmPrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(prefix + AdpcmPrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(prefix + AdpcmPrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(prefix + AdpcmPrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(prefix + AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(prefix + AdpcmPrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(prefix + AdpcmPrKey::ssgSwEnv + AdpcmPrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(prefix + AdpcmPrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(prefix + AdpcmPrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(prefix + AdpcmPrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(prefix + AdpcmPrKey::dt);
    pDetune2 = apvts.getRawParameterValue(prefix + AdpcmPrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(prefix + AdpcmPrKey::dt3);

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
}

void AdpcmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
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
    params.adpcm.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread;

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

    params.adpcm.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.adpcm.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.adpcm.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.adpcm.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.adpcm.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.adpcm.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > AdpcmPrValue::boolThread);
    params.adpcm.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.adpcm.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.adpcm.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.adpcm.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.adpcm.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.adpcm.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
}
