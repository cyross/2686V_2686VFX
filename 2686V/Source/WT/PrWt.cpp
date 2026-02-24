#include "PrWt.h"

void WtProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(codeWt + postBit, codeWt + mPostBitTitle, mBitMin, mBitMax, wtBitDefault));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(codeWt + postRate, codeWt + mPostRateTitle, mRateMin, mRateMax, wtRateDefault)); // Default 6 (16kHz)
    // Size: 0:32, 1:64
    layout.add(std::make_unique<juce::AudioParameterInt>(codeWt + postSize, codeWt + wtPostSampleSize, wtSampleSizeMin, wtSampleSizeMax, wtSampleSizeDefault));
    // Waveform Preset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(codeWt + postWave, codeWt + wtPostWaveform, wtWaveformMin, wtWaveformMax, wtWaveformDefault));

    // Custom Waveform Sliders (32 params)
    // Range: -1.0 to 1.0, Default: 0.0
    for (int i = 0; i < wtCustomSize1; ++i)
    {
        auto paramId = codeWt + codeCustom32 + juce::String(i);
        auto paramName = codeWt + wtCodeCustom32 + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, wtCustomMin, wtCustomMax, wtCustomDefault));
    }

    for (int i = 0; i < wtCustomSize2; ++i)
    {
        auto paramId = codeWt + codeCustom64 + juce::String(i);
        auto paramName = codeWt + wtCodeCustom64 + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, wtCustomMin, wtCustomMax, wtCustomDefault));
    }

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(codeWt + postModEnable, codeWt + wtPostModEnable, wtModEnableDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeWt + postModDepth, codeWt + wtPostModDepth, wtModDepthMin, wtModDepthMax, wtModDepthDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeWt + postModSpeed, codeWt + wtPostModSpeed, wtModSpeedMin, wtModSpeedMax, wtModSpeedDefault));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(codeWt + postLevel, codeWt + wtPostLevel, wtLevelMin, wtLevelMax, wtLevelDefault));

    addEnvParameters(layout, codeWt);
}

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.wtBitDepth = (int)*apvts.getRawParameterValue(codeWt + postBit);
    params.wtRateIndex = (int)*apvts.getRawParameterValue(codeWt + postRate);
    params.wtTableSize = (int)*apvts.getRawParameterValue(codeWt + postSize);
    params.wtWaveform = (int)*apvts.getRawParameterValue(codeWt + postWave);

    // Custom Wave params reading
    for (int i = 0; i < wtCustomSize1; ++i)
    {
        auto paramId = codeWt + codeCustom32 + juce::String(i);
        params.wtCustomWave32[i] = *apvts.getRawParameterValue(paramId);
    }

    // Custom Wave params reading
    for (int i = 0; i < wtCustomSize2; ++i)
    {
        auto paramId = codeWt + codeCustom64 + juce::String(i);
        params.wtCustomWave64[i] = *apvts.getRawParameterValue(paramId);
    }

    params.wtModEnable = (*apvts.getRawParameterValue(codeWt + postModEnable) > opBoolThread);
    params.wtModDepth = *apvts.getRawParameterValue(codeWt + postModDepth);
    params.wtModSpeed = *apvts.getRawParameterValue(codeWt + postModSpeed);
    params.wtLevel = *apvts.getRawParameterValue(codeWt + postLevel);

    params.wtAdsr.a = *apvts.getRawParameterValue(codeWt + postAr);
    params.wtAdsr.d = *apvts.getRawParameterValue(codeWt + postDr);
    params.wtAdsr.s = *apvts.getRawParameterValue(codeWt + postSl);
    params.wtAdsr.r = *apvts.getRawParameterValue(codeWt + postRr);
}
