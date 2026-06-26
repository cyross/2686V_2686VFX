#include "./ProcessorRhythm.h"

#include "./ProcessorRhythmKeys.h"
#include "./ProcessorRhythmValues.h"
#include "./ProcessorRhythmNames.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String prefix = RhythmPrKey::prefix;
    const juce::String prefixName = RhythmPrName::prefix;

    // ==========================================
    // Rhythm Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + RhythmPrKey::level, prefixName + RhythmPrName::vol, RhythmPrValue::Level::min, RhythmPrValue::Level::max, RhythmPrValue::Level::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + RhythmPrKey::Unison::voices, prefixName + RhythmPrName::Unison::voices, RhythmPrValue::Unison::Voices::min, RhythmPrValue::Unison::Voices::max, RhythmPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + RhythmPrKey::Unison::detune, prefixName + RhythmPrName::Unison::detune, RhythmPrValue::Unison::Detune::min, RhythmPrValue::Unison::Detune::max, RhythmPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + RhythmPrKey::Unison::spread, prefixName + RhythmPrName::Unison::spread, RhythmPrValue::Unison::Spread::min, RhythmPrValue::Unison::Spread::max, RhythmPrValue::Unison::Spread::initial));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        juce::String padPrefix = prefix + RhythmPrKey::pad + juce::String(i);
        juce::String padPrefixName = prefixName + RhythmPrName::pad + juce::String(i + 1);

        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::volume, padPrefixName + RhythmPrName::Pad::vol, RhythmPrValue::Level::min, RhythmPrValue::Level::max, RhythmPrValue::Level::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::tone, padPrefixName + RhythmPrName::Pad::tone, RhythmPrValue::Pad::Tone::min, RhythmPrValue::Pad::Tone::max, RhythmPrValue::Pad::Tone::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::noise, padPrefixName + RhythmPrName::Pad::noise, RhythmPrValue::Pad::Noise::min, RhythmPrValue::Pad::Noise::max, RhythmPrValue::Pad::Noise::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::noiseFreq, padPrefixName + RhythmPrName::Pad::noiseFreq, RhythmPrValue::Pad::NoiseFreq::min, RhythmPrValue::Pad::NoiseFreq::max, RhythmPrValue::Pad::NoiseFreq::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::mix, padPrefixName + RhythmPrName::Pad::mix, RhythmPrValue::Pad::Mix::min, RhythmPrValue::Pad::Mix::max, RhythmPrValue::Pad::Mix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::pan, padPrefixName + RhythmPrName::Pad::pan, RhythmPrValue::Pad::Pan::min, RhythmPrValue::Pad::Pan::max, RhythmPrValue::Pad::Pan::initial));

        // Fix
        layout.add(std::make_unique<juce::AudioParameterBool>(padPrefix + RhythmPrKey::Pad::fix, padPrefixName + RhythmPrName::Pad::fix, RhythmPrValue::Pad::Fix::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::fixFreq, padPrefixName + RhythmPrName::Pad::fixFreq, RhythmPrValue::Pad::FixFreq::min, RhythmPrValue::Pad::FixFreq::max, RhythmPrValue::Pad::FixFreq::initial));

        // Note number
        int defNote = RhythmPrValue::Pad::Note::initial + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::note, padPrefixName + RhythmPrName::Pad::note, RhythmPrValue::Pad::Note::min, RhythmPrValue::Pad::Note::max, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::mode, padPrefixName + RhythmPrName::Pad::bit, RhythmPrValue::Pad::Bit::min, RhythmPrValue::Pad::Bit::max, RhythmPrValue::Pad::Bit::initial));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::rate, padPrefixName + RhythmPrName::Pad::rate, RhythmPrValue::Pad::Rate::min, RhythmPrValue::Pad::Rate::max, RhythmPrValue::Pad::Rate::initial));

        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::interp, padPrefixName + RhythmPrName::Pad::interp, RhythmPrValue::Pad::Interp::min, RhythmPrValue::Pad::Interp::max, RhythmPrValue::Pad::Interp::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(padPrefix + RhythmPrKey::Pad::oneShot, padPrefixName + RhythmPrName::Pad::oneShot, RhythmPrValue::Pad::OneShot::initial));

        // ADSR Bypass Switch
        layout.add(std::make_unique<juce::AudioParameterBool>(padPrefix + RhythmPrKey::adsr + RhythmPrKey::bypass, padPrefixName + RhythmPrName::Adsr::bypass, RhythmPrValue::Pad::Adsr::Bypass::initial));

        // PitchEnv Bypass Switch
        layout.add(std::make_unique<juce::AudioParameterBool>(padPrefix + RhythmPrKey::pitchAdsr + RhythmPrKey::bypass, padPrefixName + RhythmPrName::PitchAdsr::bypass, RhythmPrValue::Pad::PitchAdsr::Bypass::initial));

        // SSG SwEnv Bypass Switch
        layout.add(std::make_unique<juce::AudioParameterBool>(padPrefix + RhythmPrKey::ssgSwEnv + RhythmPrKey::bypass, padPrefixName + RhythmPrName::SsgSwEnv::bypass, RhythmPrValue::Pad::SsgSwEnv::Bypass::initial));

        // Detune
        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::mul, padPrefixName + RhythmPrName::mul, RhythmPrValue::Pad::Mul::min, RhythmPrValue::Pad::Mul::max, RhythmPrValue::Pad::Mul::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::mulRatio, padPrefixName + RhythmPrName::mulRatio, RhythmPrValue::Pad::MulRatio::min, RhythmPrValue::Pad::MulRatio::max, RhythmPrValue::Pad::MulRatio::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::dt, padPrefixName + RhythmPrName::dt1, RhythmPrValue::Pad::Dt1::min, RhythmPrValue::Pad::Dt1::max, RhythmPrValue::Pad::Dt1::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::dt2, padPrefixName + RhythmPrName::dt2, RhythmPrValue::Pad::Dt2::min, RhythmPrValue::Pad::Dt2::max, RhythmPrValue::Pad::Dt2::initial));
        layout.add(std::make_unique<juce::AudioParameterInt>(padPrefix + RhythmPrKey::Pad::dt3, padPrefixName + RhythmPrName::dt3, RhythmPrValue::Pad::Dt3::min, RhythmPrValue::Pad::Dt3::max, RhythmPrValue::Pad::Dt3::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::pcmOffset, padPrefixName + RhythmPrName::Pad::pcmOffset, RhythmPrValue::Pad::Offset::min, RhythmPrValue::Pad::Offset::max, RhythmPrValue::Pad::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::pcmRatio, padPrefixName + RhythmPrName::Pad::pcmRatio, RhythmPrValue::Pad::Ratio::min, RhythmPrValue::Pad::Ratio::max, RhythmPrValue::Pad::Ratio::initial));
        layout.add(std::make_unique<juce::AudioParameterBool>(padPrefix + RhythmPrKey::Pad::loopPointEnable, padPrefixName + RhythmPrName::Pad::loopPointEnable, RhythmPrValue::Pad::LoopPointEnable::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::loopPointStart, padPrefixName + RhythmPrName::Pad::loopPointStart, RhythmPrValue::Pad::LoopPointStart::min, RhythmPrValue::Pad::LoopPointStart::max, RhythmPrValue::Pad::LoopPointStart::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::loopPointEnd, padPrefixName + RhythmPrName::Pad::loopPointEnd, RhythmPrValue::Pad::LoopPointEnd::min, RhythmPrValue::Pad::LoopPointEnd::max, RhythmPrValue::Pad::LoopPointEnd::initial));

        addOpEnvParameters(layout, padPrefix, padPrefixName);
        addOpPitchEnvParameters(layout, padPrefix, padPrefixName);
        addSsgSwEnvParameters(layout, padPrefix, padPrefixName);
        addOpzx7LfoParameters(layout, padPrefix, padPrefixName);
    }
}

void RhythmProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String prefix = RhythmPrKey::prefix;

    pMLevel = apvts.getRawParameterValue(prefix + RhythmPrKey::level);
    pUnisonVoices = apvts.getRawParameterValue(prefix + RhythmPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(prefix + RhythmPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(prefix + RhythmPrKey::Unison::spread);
    
    for (int i = 0; i < RhythmPrValue::pads; i++) {
        juce::String padPrefix = prefix + RhythmPrKey::pad + juce::String(i);

        pLevel[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::volume);
        pTone[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::tone);
        pNoise[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::noise);
        pNoiseFreq[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::noiseFreq);
        pMix[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::mix);
        pPan[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::pan);
        pNoteNumber[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::note);
        pQualityMode[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::mode);
        pRateIndex[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::rate);
        pInterpolationMode[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::interp);
        pIsOneShot[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::oneShot);
        pPcmOffset[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::pcmOffset);
        pPcmRatio[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::pcmRatio);
        pLoopPointEnable[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::loopPointEnable);
        pLoopPointStart[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::loopPointStart);
        pLoopPointEnd[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::loopPointEnd);

        pFixMode[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::fix);
        pFixFreq[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::fixFreq);

        pAdsrBypass[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::adsr + RhythmPrKey::bypass);
        pAdsrStl[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::Adsr::stl);
        pAdsrAr[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::Adsr::ar);
        pAdsrDr[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::Adsr::dr);
        pAdsrSl[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::Adsr::sl);
        pAdsrRr[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::Adsr::rr);
        pAdsrKor[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::Adsr::kor);

        pPitchAdsrBypass[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::pitchAdsr + RhythmPrKey::bypass);
        pPitchAdsrAr[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::ar);
        pPitchAdsrDr[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::dr);
        pPitchAdsrRr[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::rr);
        pPitchAdsrStl[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::stl);
        pPitchAdsrAtl[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::atl);
        pPitchAdsrSsl[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::ssl);
        pPitchAdsrRll[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::PitchAdsr::rll);

        pSsgSwEnvBypass[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::ssgSwEnv + RhythmPrKey::bypass);
        pSsgSwEnvSteps[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::steps);
        pSsgSwEnvLoop[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::loop);
        pSsgSwEnvLoopTo[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::loopTo);
        pSsgSwEnvLoopCount[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::loopCount);
        pSsgSwEnvR1[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::r1);
        pSsgSwEnvR2[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::r2);
        pSsgSwEnvR3[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::r3);
        pSsgSwEnvR4[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::r4);
        pSsgSwEnvR5[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::r5);
        pSsgSwEnvR6[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::r6);
        pSsgSwEnvStl[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::stl);
        pSsgSwEnvL1[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::l1);
        pSsgSwEnvL2[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::l2);
        pSsgSwEnvL3[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::l3);
        pSsgSwEnvL4[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::l4);
        pSsgSwEnvL5[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::l5);
        pSsgSwEnvL6[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::SsgSwEnv::l6);

        pMultiple[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::mul);
        pMultipleRatio[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::mulRatio);
        pDetune[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::dt);
        pDetune2[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::dt2);
        pDetune3[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::dt3);

        pLfoPmSyncDelay[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::pmSyncDelay);
        pLfoAmSyncDelay[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::amSyncDelay);
        pLfoAmSmoothRatio[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::amSmoothRatio);
        pLfoPmFreq[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::pmFreq);
        pLfoAmFreq[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::amFreq);
        pLfoPmShape[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::pgShape);
        pLfoAmShape[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::egShape);
        pLfoPm[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::pm);
        pLfoAm[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::am);
        pLfoPmd[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::pmd);
        pLfoPms[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::pms);
        pLfoAmd[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::amd);
        pLfoAms[i] = apvts.getRawParameterValue(padPrefix + CorePrKey::Post::Lfo::ams);
    }
}

void RhythmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // --- Rhythm Parameters ---
    params.rhythm.level = pMLevel->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.rhythm.unisonVoices = (int)pMLevel->load(std::memory_order_relaxed);
    params.rhythm.unisonDetuneCents = (int)pMLevel->load(std::memory_order_relaxed);
    params.rhythm.unisonSpread = pMLevel->load(std::memory_order_relaxed);

    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        auto& pad = params.rhythm.pads[i];
        pad.level = pLevel[i]->load(std::memory_order_relaxed);
        pad.tone = pTone[i]->load(std::memory_order_relaxed);
        pad.noiseLevel = pNoise[i]->load(std::memory_order_relaxed);
        pad.noiseFreq = pNoiseFreq[i]->load(std::memory_order_relaxed);
        pad.mix = pMix[i]->load(std::memory_order_relaxed);
        pad.pan = pPan[i]->load(std::memory_order_relaxed);
        pad.noteNumber = (int)pNoteNumber[i]->load(std::memory_order_relaxed);
        pad.qualityMode = (int)pQualityMode[i]->load(std::memory_order_relaxed);
        pad.rateIndex = (int)pRateIndex[i]->load(std::memory_order_relaxed);
        pad.interpolationMode = (int)pInterpolationMode[i]->load(std::memory_order_relaxed);
        pad.isOneShot = (bool)pIsOneShot[i]->load(std::memory_order_relaxed);
        pad.pcmOffset = pPcmOffset[i]->load(std::memory_order_relaxed);
        pad.pcmRatio = pPcmRatio[i]->load(std::memory_order_relaxed);
        pad.loopPointEnable = (pLoopPointEnable[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.loopPointStart = pLoopPointStart[i]->load(std::memory_order_relaxed);
        pad.loopPointEnd = pLoopPointEnd[i]->load(std::memory_order_relaxed);

        pad.fixedMode = (pFixMode[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.fixedFreq = pFixFreq[i]->load(std::memory_order_relaxed);

        pad.adsr.bypass = (pAdsrBypass[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.adsr.stl = pAdsrStl[i]->load(std::memory_order_relaxed);
        pad.adsr.ar = pAdsrAr[i]->load(std::memory_order_relaxed);
        pad.adsr.dr = pAdsrDr[i]->load(std::memory_order_relaxed);
        pad.adsr.sl = pAdsrSl[i]->load(std::memory_order_relaxed);
        pad.adsr.rr = pAdsrRr[i]->load(std::memory_order_relaxed);
        pad.adsr.kor = pAdsrKor[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread;

        pad.pitchAdsr.bypass = (pPitchAdsrBypass[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.pitchAdsr.ar = pPitchAdsrAr[i]->load(std::memory_order_relaxed);
        pad.pitchAdsr.dr = pPitchAdsrDr[i]->load(std::memory_order_relaxed);
        pad.pitchAdsr.rr = pPitchAdsrRr[i]->load(std::memory_order_relaxed);
        pad.pitchAdsr.stl = (int)pPitchAdsrStl[i]->load(std::memory_order_relaxed);
        pad.pitchAdsr.atl = (int)pPitchAdsrAtl[i]->load(std::memory_order_relaxed);
        pad.pitchAdsr.ssl = (int)pPitchAdsrRr[i]->load(std::memory_order_relaxed);
        pad.pitchAdsr.rll = (int)pPitchAdsrRll[i]->load(std::memory_order_relaxed);

        pad.ssgSwEnv.bypass = (pSsgSwEnvBypass[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.ssgSwEnv.steps = (int)pSsgSwEnvSteps[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.loop = (pSsgSwEnvLoop[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.stl = pSsgSwEnvStl[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.r1 = pSsgSwEnvR1[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.l1 = pSsgSwEnvL1[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.r2 = pSsgSwEnvR2[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.l2 = pSsgSwEnvL2[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.r3 = pSsgSwEnvR3[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.l3 = pSsgSwEnvL3[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.r4 = pSsgSwEnvR4[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.l4 = pSsgSwEnvL4[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.l5 = pSsgSwEnvL5[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.r6 = pSsgSwEnvR6[i]->load(std::memory_order_relaxed);
        pad.ssgSwEnv.l6 = pSsgSwEnvL6[i]->load(std::memory_order_relaxed);

        pad.multiple = (int)pMultiple[i]->load(std::memory_order_relaxed);
        pad.multipleRatio = pMultipleRatio[i]->load(std::memory_order_relaxed);
        pad.detune = (int)pDetune[i]->load(std::memory_order_relaxed);
        pad.detune2 = (int)pDetune2[i]->load(std::memory_order_relaxed);
        pad.detune3 = (int)pDetune3[i]->load(std::memory_order_relaxed);

        pad.lfoPmFreq = pLfoPmFreq[i]->load(std::memory_order_relaxed);
        pad.lfoAmFreq = pLfoAmFreq[i]->load(std::memory_order_relaxed);
        pad.lfoPmWave = (int)pLfoPmShape[i]->load(std::memory_order_relaxed);
        pad.lfoAmWave = (int)pLfoAmShape[i]->load(std::memory_order_relaxed);
        pad.lfoAmSmRt = pLfoAmSmoothRatio[i]->load(std::memory_order_relaxed);
        pad.lfoAmEnable = (pLfoAm[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.lfoPmEnable = (pLfoPm[i]->load(std::memory_order_relaxed) > RhythmPrValue::boolThread);
        pad.lfoPms = pLfoPms[i]->load(std::memory_order_relaxed);
        pad.lfoAms = pLfoAms[i]->load(std::memory_order_relaxed);
        pad.lfoPmd = pLfoPmd[i]->load(std::memory_order_relaxed);
        pad.lfoAmd = pLfoAmd[i]->load(std::memory_order_relaxed);
        pad.lfoPmSyncDelay = pLfoPmSyncDelay[i]->load(std::memory_order_relaxed);
        pad.lfoAmSyncDelay = pLfoAmSyncDelay[i]->load(std::memory_order_relaxed);
    }
}
