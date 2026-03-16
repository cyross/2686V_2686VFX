#include "PrRhythm.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::rhythm;

    // ==========================================
    // Rhythm Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Rhythm::level, code + PrName::Rhythm::vol, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < PrValue::Rhythm::pads; ++i) {
        juce::String prefix = code + PrKey::Innder::pad + juce::String(i);
        juce::String namePrefix = code + PrName::Rhythm::Pad::key + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Rhythm::Pad::volume, namePrefix + PrName::Rhythm::Pad::Post::vol, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Rhythm::Pad::pan, namePrefix + PrName::Rhythm::Pad::Post::pan, PrValue::Pan::min, PrValue::Pan::max, PrValue::Pan::initial));

        // Note number
        int defNote = PrValue::Rhythm::Pad::Note::initial + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Rhythm::Pad::note, namePrefix + PrName::Rhythm::Pad::Post::note, PrValue::Rhythm::Pad::Note::min, PrValue::Rhythm::Pad::Note::max, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Rhythm::Pad::mode, namePrefix + PrName::Rhythm::Pad::Post::bit, PrValue::Pcm::Bit::min, PrValue::Pcm::Bit::max, PrValue::Pcm::Bit::initial));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::Rhythm::Pad::rate, namePrefix + PrName::Rhythm::Pad::Post::rate, PrValue::Pcm::Rate::min, PrValue::Pcm::Rate::max, PrValue::Pcm::Rate::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Post::Rhythm::Pad::oneShot, namePrefix + PrName::Rhythm::Pad::Post::oneShot, PrValue::Rhythm::Pad::OneShot::initial));

        // Release Parameter
        // 範囲: 0.03秒 ～ 5.0秒, 初期値: 0.1秒
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Rhythm::Pad::rr, namePrefix + PrName::Rhythm::Pad::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Rhythm::Pad::pcmOffset, namePrefix + PrName::Rhythm::Pad::Post::pcmOffset, PrValue::Pcm::Offset::min, PrValue::Pcm::Offset::max, PrValue::Pcm::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Rhythm::Pad::pcmRatio, namePrefix + PrName::Rhythm::Pad::Post::pcmRatio, PrValue::Pcm::Ratio::min, PrValue::Pcm::Ratio::max, PrValue::Pcm::Ratio::initial));
    }
}

void RhythmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::rhythm;

    // --- Rhythm Parameters ---
    params.rhythmLevel = *apvts.getRawParameterValue(code + PrKey::Post::Rhythm::level);

    for (int i = 0; i < PrValue::Rhythm::pads; ++i) {
        juce::String prefix = code + PrKey::Innder::pad + juce::String(i);
        auto& pad = params.rhythmPads[i];
        pad.level = *apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::volume);
        pad.pan = *apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::pan);
        pad.noteNumber = (int)*apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::note);
        pad.qualityMode = (int)*apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::mode);
        pad.rateIndex = (int)*apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::rate);
        pad.isOneShot = (bool)*apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::oneShot);
        pad.release = *apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::rr);
        pad.pcmOffset = *apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::pcmOffset);
        pad.pcmRatio = *apvts.getRawParameterValue(prefix + PrKey::Post::Rhythm::Pad::pcmRatio);
    }
}
