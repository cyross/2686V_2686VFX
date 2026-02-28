#include "PrWt.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void WtProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::wt;
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::bit, code + PrName::Wt::Post::bit, PrValue::Wt::BIt::min, PrValue::Wt::BIt::max, PrValue::Wt::BIt::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::rate, code + PrName::Fm::Post::rate, PrValue::Wt::Rate::min, PrValue::Wt::Rate::max, PrValue::Wt::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::sampleSize, code + PrName::Wt::Post::sampleSize, PrValue::Wt::SammpleSize::min, PrValue::Wt::SammpleSize::max, PrValue::Wt::SammpleSize::initial));
    // Waveform Preset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::wave, code + PrName::Wt::Post::waveform, PrValue::Wt::WaveForm::min, PrValue::Wt::WaveForm::max, PrValue::Wt::WaveForm::initial));

    // Custom Waveform Sliders (32 params)
    // Range: -1.0 to 1.0, Default: 0.0
    for (int i = 0; i < PrValue::Wt::customSize1; ++i)
    {
        auto paramId = code + PrKey::Innder::custom32 + juce::String(i);
        auto paramName = code + PrName::Wt::Key::custom32 + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, PrValue::Wt::Custom::min, PrValue::Wt::Custom::max, PrValue::Wt::Custom::initial));
    }

    for (int i = 0; i < PrValue::Wt::customSize2; ++i)
    {
        auto paramId = code + PrKey::Innder::custom64 + juce::String(i);
        auto paramName = code + PrName::Wt::Key::custom64 + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, PrValue::Wt::Custom::min, PrValue::Wt::Custom::max, PrValue::Wt::Custom::initial));
    }

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Wt::Mod::enable, code + PrName::Wt::Post::Mod::enable, PrValue::Wt::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Wt::Mod::depth, code + PrName::Wt::Post::Mod::depth, PrValue::Wt::Mod::Depth::min, PrValue::Wt::Mod::Depth::max, PrValue::Wt::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Wt::Mod::speed, code + PrName::Wt::Post::Mod::speed, PrValue::Wt::Mod::Speed::min, PrValue::Wt::Mod::Speed::max, PrValue::Wt::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Wt::level, code + PrName::Wt::Post::level, PrValue::Wt::Level::min, PrValue::Wt::Level::max, PrValue::Wt::Level::initial));

    addEnvParameters(layout, code);
}

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::wt;
        
    params.wtBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::bit);
    params.wtRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::rate);
    params.wtTableSize = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::sampleSize);
    params.wtWaveform = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::wave);

    // Custom Wave params reading
    for (int i = 0; i < PrValue::Wt::customSize1; ++i)
    {
        auto paramId = code + PrKey::Innder::custom32 + juce::String(i);
        params.wtCustomWave32[i] = *apvts.getRawParameterValue(paramId);
    }

    // Custom Wave params reading
    for (int i = 0; i < PrValue::Wt::customSize1; ++i)
    {
        auto paramId = code + PrKey::Innder::custom32 + juce::String(i);
        params.wtCustomWave64[i] = *apvts.getRawParameterValue(paramId);
    }

    params.wtModEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Wt::Mod::enable) > PrValue::boolThread);
    params.wtModDepth = *apvts.getRawParameterValue(code + PrKey::Post::Wt::Mod::depth);
    params.wtModSpeed = *apvts.getRawParameterValue(code + PrKey::Post::Wt::Mod::speed);
    params.wtLevel = *apvts.getRawParameterValue(code + PrKey::Post::Wt::level);

    params.wtAdsr.a = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::ar);
    params.wtAdsr.d = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::dr);
    params.wtAdsr.s = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::sl);
    params.wtAdsr.r = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::rr);
}
