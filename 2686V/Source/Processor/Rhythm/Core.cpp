#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // ==========================================
    // Rhythm Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::level, code + PrName::vol, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < PrValue::pads; ++i) {
        juce::String prefix = code + PrKey::pad + juce::String(i);
        juce::String namePrefix = code + PrName::pad + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Pad::volume, namePrefix + PrName::Pad::vol, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Pad::pan, namePrefix + PrName::Pad::pan, PrValue::Pad::Pan::min, PrValue::Pad::Pan::max, PrValue::Pad::Pan::initial));

        // Note number
        int defNote = PrValue::Pad::Note::initial + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Pad::note, namePrefix + PrName::Pad::note, PrValue::Pad::Note::min, PrValue::Pad::Note::max, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Pad::mode, namePrefix + PrName::Pad::bit, PrValue::Pad::Bit::min, PrValue::Pad::Bit::max, PrValue::Pad::Bit::initial));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Pad::rate, namePrefix + PrName::Pad::rate, PrValue::Pad::Rate::min, PrValue::Pad::Rate::max, PrValue::Pad::Rate::initial));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + PrKey::Pad::oneShot, namePrefix + PrName::Pad::oneShot, PrValue::Pad::OneShot::initial));

        // Release Parameter
        // 範囲: 0.03秒 ～ 5.0秒, 初期値: 0.1秒
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Pad::rr, namePrefix + PrName::Pad::rr, PrValue::Pad::Adsr::Rr::min, PrValue::Pad::Adsr::Rr::max, PrValue::Pad::Adsr::Rr::initial));

        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Pad::pcmOffset, namePrefix + PrName::Pad::pcmOffset, PrValue::Pad::Offset::min, PrValue::Pad::Offset::max, PrValue::Pad::Offset::initial));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Pad::pcmRatio, namePrefix + PrName::Pad::pcmRatio, PrValue::Pad::Ratio::min, PrValue::Pad::Ratio::max, PrValue::Pad::Ratio::initial));
    }
}

void RhythmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    // --- Rhythm Parameters ---
    params.rhythm.level = *apvts.getRawParameterValue(code + PrKey::level);

    for (int i = 0; i < PrValue::pads; ++i) {
        juce::String prefix = code + PrKey::pad + juce::String(i);
        auto& pad = params.rhythm.pads[i];
        pad.level = *apvts.getRawParameterValue(prefix + PrKey::Pad::volume);
        pad.pan = *apvts.getRawParameterValue(prefix + PrKey::Pad::pan);
        pad.noteNumber = (int)*apvts.getRawParameterValue(prefix + PrKey::Pad::note);
        pad.qualityMode = (int)*apvts.getRawParameterValue(prefix + PrKey::Pad::mode);
        pad.rateIndex = (int)*apvts.getRawParameterValue(prefix + PrKey::Pad::rate);
        pad.isOneShot = (bool)*apvts.getRawParameterValue(prefix + PrKey::Pad::oneShot);
        pad.release = *apvts.getRawParameterValue(prefix + PrKey::Pad::rr);
        pad.pcmOffset = *apvts.getRawParameterValue(prefix + PrKey::Pad::pcmOffset);
        pad.pcmRatio = *apvts.getRawParameterValue(prefix + PrKey::Pad::pcmRatio);
    }
}
