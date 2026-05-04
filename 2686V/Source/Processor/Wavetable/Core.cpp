#include "./Core.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void WtProcessor::createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& code, const juce::String& name)
{
    for (int i = 0; i < size; ++i)
    {
        auto paramId = code + juce::String(i);
        auto paramName = name + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, PrValue::Wt::Custom::min, PrValue::Wt::Custom::max, PrValue::Wt::Custom::initial));
    }

}

template <size_t I>
void WtProcessor::processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
{
    // Custom Wave params reading
    for (int i = 0; i < I; ++i)
    {
        auto paramId = code + juce::String(i);
        samples[i] = *apvts.getRawParameterValue(paramId);
    }

}

void WtProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::wt;
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::bit, code + PrName::Wt::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::rate, code + PrName::Fm::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::sampleSize, code + PrName::Wt::Post::sampleSize, PrValue::Wt::SammpleSize::min, PrValue::Wt::SammpleSize::max, PrValue::Wt::SammpleSize::initial));
    // Waveform Preset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Wt::wave, code + PrName::Wt::Post::waveform, PrValue::Wt::WaveForm::min, PrValue::Wt::WaveForm::max, PrValue::Wt::WaveForm::initial));

    createCustomWaveLayout(layout, PrValue::Wt::customSize1, code + PrKey::Innder::custom32, code + PrName::Wt::Key::custom32);
    createCustomWaveLayout(layout, PrValue::Wt::customSize2, code + PrKey::Innder::custom64, code + PrName::Wt::Key::custom64);
    createCustomWaveLayout(layout, PrValue::Wt::customSize3, code + PrKey::Innder::custom128, code + PrName::Wt::Key::custom128);
    createCustomWaveLayout(layout, PrValue::Wt::customSize4, code + PrKey::Innder::custom256, code + PrName::Wt::Key::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Wt::Mod::enable, code + PrName::Wt::Post::Mod::enable, PrValue::Wt::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Wt::Mod::depth, code + PrName::Wt::Post::Mod::depth, PrValue::Wt::Mod::Depth::min, PrValue::Wt::Mod::Depth::max, PrValue::Wt::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Wt::Mod::speed, code + PrName::Wt::Post::Mod::speed, PrValue::Wt::Mod::Speed::min, PrValue::Wt::Mod::Speed::max, PrValue::Wt::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Wt::level, code + PrName::Wt::Post::level, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));

    addEnvParameters(layout, code);
}

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::wt;
        
    params.wt.bitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::bit);
    params.wt.rateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::rate);
    params.wt.tableSize = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::sampleSize);
    params.wt.waveform = (int)*apvts.getRawParameterValue(code + PrKey::Post::Wt::wave);

    processCustomWaveBlock(params.wt.customWave32, apvts, code + PrKey::Innder::custom32);
    processCustomWaveBlock(params.wt.customWave64, apvts, code + PrKey::Innder::custom64);
    processCustomWaveBlock(params.wt.customWave128, apvts, code + PrKey::Innder::custom128);
    processCustomWaveBlock(params.wt.customWave256, apvts, code + PrKey::Innder::custom256);

    params.wt.modEnable = (*apvts.getRawParameterValue(code + PrKey::Post::Wt::Mod::enable) > PrValue::boolThread);
    params.wt.modDepth = *apvts.getRawParameterValue(code + PrKey::Post::Wt::Mod::depth);
    params.wt.modSpeed = *apvts.getRawParameterValue(code + PrKey::Post::Wt::Mod::speed);
    params.wt.level = *apvts.getRawParameterValue(code + PrKey::Post::Wt::level);

    params.wt.adsr.bypass = false;
    params.wt.adsr.ar = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::ar);
    params.wt.adsr.dr = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::dr);
    params.wt.adsr.sl = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::sl);
    params.wt.adsr.rr = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::rr);
}
