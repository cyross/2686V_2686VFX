#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = RhythmPrKey::prefix;

    // ==========================================
    // Rhythm Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + RhythmPrKey::level, code + RhythmPrName::vol, RhythmPrValue::Level::min, RhythmPrValue::Level::max, RhythmPrValue::Level::initial));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        juce::String prefix = code + RhythmPrKey::pad + juce::String(i);
        juce::String namePrefix = code + RhythmPrName::pad + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + RhythmPrKey::Pad::volume, namePrefix + RhythmPrName::Pad::vol, RhythmPrValue::Level::min, RhythmPrValue::Level::max, RhythmPrValue::Level::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + RhythmPrKey::Pad::pan, namePrefix + RhythmPrName::Pad::pan, RhythmPrValue::Pad::Pan::min, RhythmPrValue::Pad::Pan::max, RhythmPrValue::Pad::Pan::initial));

        // Note number
        int defNote = RhythmPrValue::Pad::Note::initial + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + RhythmPrKey::Pad::note, namePrefix + RhythmPrName::Pad::note, RhythmPrValue::Pad::Note::min, RhythmPrValue::Pad::Note::max, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + RhythmPrKey::Pad::mode, namePrefix + RhythmPrName::Pad::bit, RhythmPrValue::Pad::Bit::min, RhythmPrValue::Pad::Bit::max, RhythmPrValue::Pad::Bit::initial));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + RhythmPrKey::Pad::rate, namePrefix + RhythmPrName::Pad::rate, RhythmPrValue::Pad::Rate::min, RhythmPrValue::Pad::Rate::max, RhythmPrValue::Pad::Rate::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + RhythmPrKey::Pad::oneShot, namePrefix + RhythmPrName::Pad::oneShot, RhythmPrValue::Pad::OneShot::initial));

        // ADSR Bypass Switch
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + RhythmPrKey::adsr + RhythmPrKey::bypass, namePrefix + RhythmPrName::Adsr::bypass, RhythmPrValue::Pad::Adsr::Bypass::initial));

        // PitchEnv Bypass Switch
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + RhythmPrKey::pitchAdsr + RhythmPrKey::bypass, namePrefix + RhythmPrName::PitchAdsr::bypass, RhythmPrValue::Pad::PitchAdsr::Bypass::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + RhythmPrKey::Pad::pcmOffset, namePrefix + RhythmPrName::Pad::pcmOffset, RhythmPrValue::Pad::Offset::min, RhythmPrValue::Pad::Offset::max, RhythmPrValue::Pad::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + RhythmPrKey::Pad::pcmRatio, namePrefix + RhythmPrName::Pad::pcmRatio, RhythmPrValue::Pad::Ratio::min, RhythmPrValue::Pad::Ratio::max, RhythmPrValue::Pad::Ratio::initial));


        addOpEnvParameters(layout, prefix, namePrefix);
        addOpPitchEnvParameters(layout, prefix, namePrefix);
    }
}

void RhythmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = RhythmPrKey::prefix;

    // --- Rhythm Parameters ---
    params.rhythm.level = *apvts.getRawParameterValue(code + RhythmPrKey::level);

    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        juce::String prefix = code + RhythmPrKey::pad + juce::String(i);
        auto& pad = params.rhythm.pads[i];
        pad.level = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::volume);
        pad.pan = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::pan);
        pad.noteNumber = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::note);
        pad.qualityMode = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::mode);
        pad.rateIndex = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::rate);
        pad.isOneShot = (bool)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::oneShot);
        pad.pcmOffset = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::pcmOffset);
        pad.pcmRatio = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::pcmRatio);

        pad.adsr.bypass = (*apvts.getRawParameterValue(prefix + RhythmPrKey::adsr + RhythmPrKey::bypass) > RhythmPrValue::boolThread);
        pad.adsr.stl = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::Adsr::stl);
        pad.adsr.ar = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::Adsr::ar);
        pad.adsr.dr = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::Adsr::dr);
        pad.adsr.sl = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::Adsr::sl);
        pad.adsr.rr = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::Adsr::rr);

        pad.pitchAdsr.bypass = (*apvts.getRawParameterValue(prefix + RhythmPrKey::pitchAdsr + RhythmPrKey::bypass) > RhythmPrValue::boolThread);
        pad.pitchAdsr.ar = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::ar);
        pad.pitchAdsr.dr = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::dr);
        pad.pitchAdsr.rr = *apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::rr);
        pad.pitchAdsr.stl = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::stl);
        pad.pitchAdsr.atl = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::atl);
        pad.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::ssl);
        pad.pitchAdsr.rll = (int)*apvts.getRawParameterValue(prefix + RhythmPrKey::Pad::PitchAdsr::rll);
    }
}
