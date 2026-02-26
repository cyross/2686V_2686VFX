#include "PrSsg.h"

#include "../core/OpConstants.h"
#include "../core/OpValueRange.h"
#include "../core/GuiLabels.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // SSG Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postLevel, codeSsg + ssgPostTone, ssgToneMin, ssgToneMax, ssgToneDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postNoise, codeSsg + ssgPostNoise, ssgNoiseMin, ssgNoiseMax, ssgNoiseDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postNoiseFreq, codeSsg + ssgPostNoiseFreq, ssgNoiseFreqRange, ssgNoiseFreqDefault)); // Noise Frequency Default: 12000Hz (Standard PSG feel)
    layout.add(std::make_unique<juce::AudioParameterBool>(codeSsg + postNoiseOnNote, codeSsg + ssgPostNoiseOnNote, ssgNoiseOnNoteDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postMix, codeSsg + ssgPostToneNoiseMix, ssgNoiseMixMin, ssgNoiseMixMax, ssgNoiseMixDefault)); // デフォルトは 0.5 (Mix)

    layout.add(std::make_unique<juce::AudioParameterInt>(codeSsg + postWaveform, codeSsg + ssgPostWaveform, ssgWaveformMin, ssgWaveformMax, ssgWaveformMax)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(codeSsg + codeAdsr + postBypass, codeSsg + ssgPostAdsrBypass, ssgAdsrBypassDefault));

    layout.add(std::make_unique<juce::AudioParameterInt>(codeSsg + postDutyMode, codeSsg + ssgPostDutyMode, ssgDutyModeMin, ssgDutyModeMax, ssgDutyModeDefault)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>(codeSsg + postDutyPreset, codeSsg + ssgPostDutyPreset, ssgDutyPresetMin, ssgDutyPresetMax, ssgDutyPresetDefault)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postDutyVar, codeSsg + ssgPostDutyVar, ssgDutyVarMin, ssgDutyVarMax, ssgDutyVarDefault)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>(codeSsg + postDutyInv, codeSsg + ssgPostDutyInvert, ssgDutyInvertDefault)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>(codeSsg + postTriKeyTrk, codeSsg + ssgPostTriangleKeyTrack, ssgTryKeyTrackDefault)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postTriPeak, codeSsg + ssgPostTrianglePeak, ssgTryPeakMin, ssgTryPeakMax, ssgTryPeakDefault)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postTriFreq, codeSsg + ssgPostTriangleManualFreq, ssgTriangleManualFreqRange, ssgTriangleManualFreqDefault));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(codeSsg + postBit, codeSsg + mPostBitTitle, mBitMin, mBitMax, ssgBitDefault));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(codeSsg + postRate, codeSsg + mPostRateTitle, mRateMin, mRateMax, ssgRateDefault)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>(codeSsg + postEnvEnable, codeSsg + ssgPostHwEnvEnable, ssgHwEnvDefault)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>(codeSsg + postEnvShape, codeSsg + ssgPostEnvShape, ssgHwEnvShapeMin, ssgHwEnvShapeMax, ssgHwEnvShapeDefault)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeSsg + postEnvPeriod, codeSsg + ssgPostEnvSpeed, ssgHwEnvSpeedMin, ssgHwEnvSpeedMax, ssgHwEnvSpeedDefault)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)

    addEnvParameters(layout, codeSsg);
}

void SsgProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    // --- SSG Parameters ---
    params.ssgLevel = *apvts.getRawParameterValue(codeSsg + postLevel);
    params.ssgNoiseLevel = *apvts.getRawParameterValue(codeSsg + postNoise);
    params.ssgNoiseFreq = *apvts.getRawParameterValue(codeSsg + postNoiseFreq);
    params.ssgNoiseOnNote = (*apvts.getRawParameterValue(codeSsg + postNoiseOnNote) > opBoolThread);
    params.ssgMix = *apvts.getRawParameterValue(codeSsg + postMix);
    params.ssgWaveform = (int)*apvts.getRawParameterValue(codeSsg + postWaveform);
    params.ssgBitDepth = (int)*apvts.getRawParameterValue(codeSsg + postBit);
    params.ssgRateIndex = (int)*apvts.getRawParameterValue(codeSsg + postRate);

    params.ssgUseHwEnv = (*apvts.getRawParameterValue(codeSsg + postEnvEnable) > opBoolThread);
    params.ssgEnvShape = (int)*apvts.getRawParameterValue(codeSsg + postEnvShape);
    params.ssgEnvPeriod = *apvts.getRawParameterValue(codeSsg + postEnvPeriod);

    params.ssgDutyMode = (int)*apvts.getRawParameterValue(codeSsg + postDutyMode);
    params.ssgDutyPreset = (int)*apvts.getRawParameterValue(codeSsg + postDutyPreset);
    params.ssgDutyVar = *apvts.getRawParameterValue(codeSsg + postDutyVar);
    params.ssgDutyInvert = (*apvts.getRawParameterValue(codeSsg + postDutyInv) > opBoolThread);

    params.ssgTriKeyTrack = (*apvts.getRawParameterValue(codeSsg + postTriKeyTrk) > opBoolThread);
    params.ssgTriPeak = *apvts.getRawParameterValue(codeSsg + postTriPeak);
    params.ssgTriFreq = *apvts.getRawParameterValue(codeSsg + postTriFreq);

    params.ssgAdsrBypass = (*apvts.getRawParameterValue(codeSsg + codeAdsr + postBypass) > opBoolThread);
    params.ssgAdsr.a = *apvts.getRawParameterValue(codeSsg + postAr);
    params.ssgAdsr.d = *apvts.getRawParameterValue(codeSsg + postDr);
    params.ssgAdsr.s = *apvts.getRawParameterValue(codeSsg + postSl);
    params.ssgAdsr.r = *apvts.getRawParameterValue(codeSsg + postRr);
}
