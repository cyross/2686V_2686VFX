#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void WtProcessor::createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& code, const juce::String& name)
{
    for (int i = 0; i < size; ++i)
    {
        auto paramId = code + juce::String(i);
        auto paramName = name + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, PrValue::Custom::min, PrValue::Custom::max, PrValue::Custom::initial));
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
    const juce::String code = PrKey::prefix;
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::bit, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64, 2:128, 3:256
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::sampleSize, code + PrName::sampleSize, PrValue::SammpleSize::min, PrValue::SammpleSize::max, PrValue::SammpleSize::initial));
    // Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::steps, code + PrName::steps, PrValue::Steps::min, PrValue::Steps::max, PrValue::Steps::initial));
    // Waveform Preset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::wave, code + PrName::waveform, PrValue::WaveForm::min, PrValue::WaveForm::max, PrValue::WaveForm::initial));

    createCustomWaveLayout(layout, PrValue::customSize1, code + PrKey::custom32, code + PrName::custom32);
    createCustomWaveLayout(layout, PrValue::customSize2, code + PrKey::custom64, code + PrName::custom64);
    createCustomWaveLayout(layout, PrValue::customSize3, code + PrKey::custom128, code + PrName::custom128);
    createCustomWaveLayout(layout, PrValue::customSize4, code + PrKey::custom256, code + PrName::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Mod::enable, code + PrName::Mod::enable, PrValue::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Mod::depth, code + PrName::Mod::depth, PrValue::Mod::Depth::min, PrValue::Mod::Depth::max, PrValue::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Mod::speed, code + PrName::Mod::speed, PrValue::Mod::Speed::min, PrValue::Mod::Speed::max, PrValue::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::level, code + PrName::level, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::dt, code + PrName::dt1, PrValue::Dt1::min, PrValue::Dt1::max, PrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::dt2, code + PrName::dt2, PrValue::Dt2::min, PrValue::Dt2::max, PrValue::Dt2::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::adsr + PrKey::bypass, code + PrName::Adsr::bypass, PrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::pitchAdsr + PrKey::bypass, code + PrName::PitchAdsr::bypass, PrValue::PitchAdsr::Bypass::initial));

    addEnvParameters(layout, code);
	addPitchEnvParameters(layout, code);
}

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    params.wt.bitDepth = (int)*apvts.getRawParameterValue(code + PrKey::bit);
    params.wt.rateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);
    params.wt.tableSize = (int)*apvts.getRawParameterValue(code + PrKey::sampleSize);
    params.wt.steps = (int)*apvts.getRawParameterValue(code + PrKey::steps);
    params.wt.waveform = (int)*apvts.getRawParameterValue(code + PrKey::wave);

    processCustomWaveBlock(params.wt.customWave32, apvts, code + PrKey::custom32);
    processCustomWaveBlock(params.wt.customWave64, apvts, code + PrKey::custom64);
    processCustomWaveBlock(params.wt.customWave128, apvts, code + PrKey::custom128);
    processCustomWaveBlock(params.wt.customWave256, apvts, code + PrKey::custom256);

    params.wt.modEnable = (*apvts.getRawParameterValue(code + PrKey::Mod::enable) > PrValue::boolThread);
    params.wt.modDepth = *apvts.getRawParameterValue(code + PrKey::Mod::depth);
    params.wt.modSpeed = *apvts.getRawParameterValue(code + PrKey::Mod::speed);
    params.wt.level = *apvts.getRawParameterValue(code + PrKey::level);

    params.wt.adsr.bypass = (*apvts.getRawParameterValue(code + PrKey::adsr + PrKey::bypass) > PrValue::boolThread);
    params.wt.adsr.ar = *apvts.getRawParameterValue(code + PrKey::Adsr::ar);
    params.wt.adsr.dr = *apvts.getRawParameterValue(code + PrKey::Adsr::dr);
    params.wt.adsr.sl = *apvts.getRawParameterValue(code + PrKey::Adsr::sl);
    params.wt.adsr.rr = *apvts.getRawParameterValue(code + PrKey::Adsr::rr);

    params.wt.pitchAdsr.bypass = (*apvts.getRawParameterValue(code + PrKey::pitchAdsr + PrKey::bypass) > PrValue::boolThread);
    params.wt.pitchAdsr.ar = *apvts.getRawParameterValue(code + PrKey::PitchAdsr::ar);
    params.wt.pitchAdsr.dr = *apvts.getRawParameterValue(code + PrKey::PitchAdsr::dr);
    params.wt.pitchAdsr.rr = *apvts.getRawParameterValue(code + PrKey::PitchAdsr::rr);
    params.wt.pitchAdsr.stl = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::stl);
    params.wt.pitchAdsr.atl = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::atl);
    params.wt.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::ssl);
    params.wt.pitchAdsr.rll = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::rll);

    params.wt.detune = (int)*apvts.getRawParameterValue(code + PrKey::dt);
    params.wt.detune2 = (int)*apvts.getRawParameterValue(code + PrKey::dt2);
}
