#include "PrRhythm.h"

#include "../core/OpConstants.h"
#include "../core/OpValueRange.h"
#include "../core/GuiLabels.h"

void RhythmProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // Rhythm Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeRhythm + postLevel, codeRhythm + rhythmPostVol, rhythmVolMin, rhythmVolMax, rhythmVolDefault));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < 8; ++i) {
        juce::String prefix = codeRhythm + codePad + juce::String(i);
        juce::String namePrefix = codeRhythm + rhythmCodePad + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postVolume, namePrefix + rhythmPostVol, rhythmPadVolMin, rhythmPadVolMax, rhythmPadVolDefault));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postPan, namePrefix + opPostPanLabel, rhythmPadPanMin, rhythmPadPanMax, rhythmPadPanDefault));

        // Note number
        int defNote = rhythmPadNoteDefaultBase + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postNote, namePrefix + rhythmPostNote, rhythmPadNoteMin, rhythmPadNoteMax, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMode, namePrefix + rhythmPostQuality, rhythmPadBitMin, rhythmPadBitMax, rhythmPadBitDefault));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postRate, namePrefix + mPostRateTitle, rhythmPadRateMin, rhythmPadRateMax, rhythmPadRateDefault));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postOneShot, namePrefix + rhythmPostOneShot, rhythmPadOneShotDefault));

        // Release Parameter
        // 範囲: 0.03秒 ～ 5.0秒, 初期値: 0.1秒
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
    }
}

void RhythmProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // --- Rhythm Parameters ---
    params.rhythmLevel = *apvts.getRawParameterValue(codeRhythm + postLevel);

    for (int i = 0; i < 8; ++i) {
        juce::String prefix = codeRhythm + codePad + juce::String(i);
        auto& pad = params.rhythmPads[i];
        pad.level = *apvts.getRawParameterValue(prefix + postVolume);
        pad.pan = *apvts.getRawParameterValue(prefix + postPan);
        pad.noteNumber = (int)*apvts.getRawParameterValue(prefix + postNote);
        pad.qualityMode = (int)*apvts.getRawParameterValue(prefix + postMode);
        pad.rateIndex = (int)*apvts.getRawParameterValue(prefix + postRate);
        pad.isOneShot = (bool)*apvts.getRawParameterValue(prefix + postOneShot);
        pad.release = *apvts.getRawParameterValue(prefix + postRr);
    }
}
