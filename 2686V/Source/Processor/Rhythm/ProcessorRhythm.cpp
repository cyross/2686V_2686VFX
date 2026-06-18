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
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::pan, padPrefixName + RhythmPrName::Pad::pan, RhythmPrValue::Pad::Pan::min, RhythmPrValue::Pad::Pan::max, RhythmPrValue::Pad::Pan::initial));

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

        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::pcmOffset, padPrefixName + RhythmPrName::Pad::pcmOffset, RhythmPrValue::Pad::Offset::min, RhythmPrValue::Pad::Offset::max, RhythmPrValue::Pad::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(padPrefix + RhythmPrKey::Pad::pcmRatio, padPrefixName + RhythmPrName::Pad::pcmRatio, RhythmPrValue::Pad::Ratio::min, RhythmPrValue::Pad::Ratio::max, RhythmPrValue::Pad::Ratio::initial));

        addOpEnvParameters(layout, padPrefix, padPrefixName);
        addOpPitchEnvParameters(layout, padPrefix, padPrefixName);
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
        pPan[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::pan);
        pNoteNumber[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::note);
        pQualityMode[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::mode);
        pRateIndex[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::rate);
        pInterpolationMode[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::interp);
        pIsOneShot[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::oneShot);
        pPcmOffset[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::pcmOffset);
        pPcmRatio[i] = apvts.getRawParameterValue(padPrefix + RhythmPrKey::Pad::pcmRatio);
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
        pad.pan = pPan[i]->load(std::memory_order_relaxed);
        pad.noteNumber = (int)pNoteNumber[i]->load(std::memory_order_relaxed);
        pad.qualityMode = (int)pQualityMode[i]->load(std::memory_order_relaxed);
        pad.rateIndex = (int)pRateIndex[i]->load(std::memory_order_relaxed);
        pad.interpolationMode = (int)pInterpolationMode[i]->load(std::memory_order_relaxed);
        pad.isOneShot = (bool)pIsOneShot[i]->load(std::memory_order_relaxed);
        pad.pcmOffset = pPcmOffset[i]->load(std::memory_order_relaxed);
        pad.pcmRatio = pPcmRatio[i]->load(std::memory_order_relaxed);

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
    }
}
