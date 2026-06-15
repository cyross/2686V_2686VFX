#include "./ProcessorBeep.h"

#include "./ProcessorBeepKeys.h"
#include "./ProcessorBeepValues.h"
#include "./ProcessorBeepNames.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = BeepPrKey::prefix;
    const juce::String prefixName = BeepPrName::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + BeepPrKey::level, prefixName + BeepPrName::level, BeepPrValue::Level::min, BeepPrValue::Level::max, BeepPrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + BeepPrKey::fix, prefixName + BeepPrName::fix, BeepPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + BeepPrKey::fixFreq, prefixName + BeepPrName::fixFreq, BeepPrValue::FixFreq::min, BeepPrValue::FixFreq::max, BeepPrValue::FixFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + BeepPrKey::adsr + BeepPrKey::bypass, prefixName + BeepPrName::Adsr::bypass, BeepPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + BeepPrKey::pitchAdsr + BeepPrKey::bypass, prefixName + BeepPrName::PitchAdsr::bypass, BeepPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + BeepPrKey::ssgSwEnv + BeepPrKey::bypass, prefixName + BeepPrName::SsgSwEnv::bypass, BeepPrValue::SsgSwEnv::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + BeepPrKey::mul, prefixName + BeepPrName::mul, BeepPrValue::Mul::min, BeepPrValue::Mul::max, BeepPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + BeepPrKey::mulRatio, prefixName + BeepPrName::mulRatio, BeepPrValue::MulRatio::min, BeepPrValue::MulRatio::max, BeepPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + BeepPrKey::dt, prefixName + BeepPrName::dt1, BeepPrValue::Dt1::min, BeepPrValue::Dt1::max, BeepPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + BeepPrKey::dt2, prefixName + BeepPrName::dt2, BeepPrValue::Dt2::min, BeepPrValue::Dt2::max, BeepPrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + BeepPrKey::dt3, prefixName + BeepPrName::dt3, BeepPrValue::Dt3::min, BeepPrValue::Dt3::max, BeepPrValue::Dt3::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + BeepPrKey::Unison::voices, prefixName + BeepPrName::Unison::voices, BeepPrValue::Unison::Voices::min, BeepPrValue::Unison::Voices::max, BeepPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + BeepPrKey::Unison::detune, prefixName + BeepPrName::Unison::detune, BeepPrValue::Unison::Detune::min, BeepPrValue::Unison::Detune::max, BeepPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + BeepPrKey::Unison::spread, prefixName + BeepPrName::Unison::spread, BeepPrValue::Unison::Spread::min, BeepPrValue::Unison::Spread::max, BeepPrValue::Unison::Spread::initial));

    addEnvParameters(layout, prefix, prefixName);
    addPitchEnvParameters(layout, prefix, prefixName);
    addSsgSwEnvParameters(layout, prefix, prefixName);
    addOpzx7LfoParameters(layout, prefix, prefixName);
}

void BeepProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = BeepPrKey::prefix;

    pLevel = apvts.getRawParameterValue(prefix + BeepPrKey::level);
    pFixMode = apvts.getRawParameterValue(prefix + BeepPrKey::fix);
    pFixFreq = apvts.getRawParameterValue(prefix + BeepPrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(prefix + BeepPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + BeepPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + BeepPrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(prefix + BeepPrKey::adsr + BeepPrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(prefix + BeepPrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(prefix + BeepPrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(prefix + BeepPrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(prefix + BeepPrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(prefix + BeepPrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(prefix + BeepPrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(prefix + BeepPrKey::pitchAdsr + BeepPrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(prefix + BeepPrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(prefix + BeepPrKey::ssgSwEnv + BeepPrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(prefix + BeepPrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(prefix + BeepPrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(prefix + BeepPrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(prefix + BeepPrKey::dt);
    pDetune2 = apvts.getRawParameterValue(prefix + BeepPrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(prefix + BeepPrKey::dt3);

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

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.beep.level = pLevel->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.beep.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.beep.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.beep.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    params.beep.fixedMode = (pFixMode->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.fixedFreq = pFixFreq->load(std::memory_order_relaxed);
    params.beep.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.beep.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.beep.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.beep.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.beep.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);
    params.beep.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > BeepPrValue::boolThread;

    params.beep.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.beep.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.beep.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.beep.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.beep.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.beep.pitchAdsr.ssl = (int)pPitchAdsrRr->load(std::memory_order_relaxed);
    params.beep.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.beep.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.r5 = pSsgSwEnvR5->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.beep.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.beep.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.beep.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.beep.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.beep.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.beep.detune3 = (int)pDetune3->load(std::memory_order_relaxed);

    params.beep.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.beep.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.beep.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.beep.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.beep.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.beep.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > BeepPrValue::boolThread);
    params.beep.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.beep.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.beep.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.beep.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.beep.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.beep.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
}
