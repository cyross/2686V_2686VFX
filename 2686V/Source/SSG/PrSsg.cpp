#include "PrSsg.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::Prefix::ssg;

    // ==========================================
    // SSG Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::tone, code + PrName::Ssg::Post::tone, PrValue::Ssg::Tome::min, PrValue::Ssg::Tome::max, PrValue::Ssg::Tome::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::noise, code + PrName::Ssg::Post::Noise::title, PrValue::Ssg::Noise::min, PrValue::Ssg::Noise::max, PrValue::Ssg::Noise::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::noiseFreq, code + PrName::Ssg::Post::Noise::freq, PrValue::Ssg::NoiseFreq::min, PrValue::Ssg::NoiseFreq::max, PrValue::Ssg::NoiseFreq::initial)); // Noise Frequency Default: 12000Hz (Standard PSG feel)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Ssg::noiseOnNote, code + PrName::Ssg::Post::Noise::noiseOnNote, PrValue::Ssg::NoiseOnNote::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::mix, code + PrName::Ssg::Post::toneNoiseMix, PrValue::Ssg::NoiseMix::min, PrValue::Ssg::NoiseMix::max, PrValue::Ssg::NoiseMix::initial)); // デフォルトは 0.5 (Mix)

    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Ssg::wveform, code + PrName::Ssg::Post::waveform, PrValue::Ssg::waveForm::min, PrValue::Ssg::waveForm::max, PrValue::Ssg::waveForm::initial)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Innder::adsr + PrKey::Post::bypass, code + PrName::Ssg::Post::Adsr::bypass, PrValue::Adsr::Bypass::initial));

    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Ssg::Duty::mode, code + PrName::Ssg::Post::Duty::mode, PrValue::Ssg::Duty::Mode::min, PrValue::Ssg::Duty::Mode::max, PrValue::Ssg::Duty::Mode::initial)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Ssg::Duty::preset, code + PrName::Ssg::Post::Duty::preset, PrValue::Ssg::Duty::Preset::min, PrValue::Ssg::Duty::Preset::max, PrValue::Ssg::Duty::Preset::initial)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::Duty::var, code + PrName::Ssg::Post::Duty::var, PrValue::Ssg::Duty::Var::min, PrValue::Ssg::Duty::Var::max, PrValue::Ssg::Duty::Var::initial)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Ssg::Duty::inv, code + PrName::Ssg::Post::Duty::invert, PrValue::Ssg::Duty::Inv::initial)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Ssg::Tri::keyTrk, code + PrName::Ssg::Post::Tri::keyTrack, PrValue::Ssg::Tri::KeyTrack::initial)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::Tri::peak, code + PrName::Ssg::Post::Tri::peak, PrValue::Ssg::Tri::Peak::min, PrValue::Ssg::Tri::Peak::max, PrValue::Ssg::Tri::Peak::initial)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::Tri::freq, code + PrName::Ssg::Post::Tri::manualFreq, PrValue::Ssg::Tri::ManualFreq::min, PrValue::Ssg::Tri::ManualFreq::max, PrValue::Ssg::Tri::ManualFreq::initial));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Ssg::bit, code + PrName::Ssg::Post::bit, PrValue::Quality::Bit::min, PrValue::Quality::Bit::max, PrValue::Quality::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Ssg::rate, code + PrName::Ssg::Post::rate, PrValue::Quality::Rate::min, PrValue::Quality::Rate::max, PrValue::Quality::Rate::initial)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Ssg::HwEnv::enable, code + PrName::Ssg::Post::HwEnv::enable, PrValue::Ssg::HwEnv::Enable::initial)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Post::Ssg::HwEnv::shape, code + PrName::Ssg::Post::HwEnv::shape, PrValue::Ssg::HwEnv::Shape::min, PrValue::Ssg::HwEnv::Shape::max, PrValue::Ssg::HwEnv::Shape::initial)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Ssg::HwEnv::period, code + PrName::Ssg::Post::HwEnv::period, PrValue::Ssg::HwEnv::Period::min, PrValue::Ssg::HwEnv::Period::max, PrValue::Ssg::HwEnv::Period::initial)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)

    addEnvParameters(layout, code);
}

void SsgProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::Prefix::ssg;

    // --- SSG Parameters ---
    params.ssgLevel = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::tone);
    params.ssgNoiseLevel = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::noise);
    params.ssgNoiseFreq = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::noiseFreq);
    params.ssgNoiseOnNote = (*apvts.getRawParameterValue(code + PrKey::Post::Ssg::noiseOnNote) > PrValue::boolThread);
    params.ssgMix = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::mix);
    params.ssgWaveform = (int)*apvts.getRawParameterValue(code + PrKey::Post::Ssg::wveform);
    params.ssgBitDepth = (int)*apvts.getRawParameterValue(code + PrKey::Post::Ssg::bit);
    params.ssgRateIndex = (int)*apvts.getRawParameterValue(code + PrKey::Post::Ssg::rate);

    params.ssgUseHwEnv = (*apvts.getRawParameterValue(code + PrKey::Post::Ssg::HwEnv::enable) > PrValue::boolThread);
    params.ssgEnvShape = (int)*apvts.getRawParameterValue(code + PrKey::Post::Ssg::HwEnv::shape);
    params.ssgEnvPeriod = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::HwEnv::period);

    params.ssgDutyMode = (int)*apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::mode);
    params.ssgDutyPreset = (int)*apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::preset);
    params.ssgDutyVar = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::var);
    params.ssgDutyInvert = (*apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::inv) > PrValue::boolThread);

    params.ssgTriKeyTrack = (*apvts.getRawParameterValue(code + PrKey::Post::Ssg::Tri::keyTrk) > PrValue::boolThread);
    params.ssgTriPeak = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::Tri::peak);
    params.ssgTriFreq = *apvts.getRawParameterValue(code + PrKey::Post::Ssg::Tri::freq);

    params.ssgAdsrBypass = (*apvts.getRawParameterValue(code + PrKey::Innder::adsr + PrKey::Post::bypass) > PrValue::boolThread);
    params.ssgAdsr.a = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::ar);
    params.ssgAdsr.d = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::dr);
    params.ssgAdsr.s = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::sl);
    params.ssgAdsr.r = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::rr);
}
