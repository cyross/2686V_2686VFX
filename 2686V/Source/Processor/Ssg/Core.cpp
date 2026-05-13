#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    // ==========================================
    // SSG Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::tone, code + PrName::tone, PrValue::Tome::min, PrValue::Tome::max, PrValue::Tome::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::noise, code + PrName::Noise::title, PrValue::Noise::min, PrValue::Noise::max, PrValue::Noise::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::noiseFreq, code + PrName::Noise::freq, PrValue::NoiseFreq::min, PrValue::NoiseFreq::max, PrValue::NoiseFreq::initial)); // Noise Frequency Default: 12000Hz (Standard PSG feel)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::noiseOnNote, code + PrName::Noise::noiseOnNote, PrValue::NoiseOnNote::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::mix, code + PrName::toneNoiseMix, PrValue::NoiseMix::min, PrValue::NoiseMix::max, PrValue::NoiseMix::initial)); // デフォルトは 0.5 (Mix)

    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::wveform, code + PrName::waveform, PrValue::waveForm::min, PrValue::waveForm::max, PrValue::waveForm::initial)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::adsr + PrKey::bypass, code + PrName::Adsr::bypass, PrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::pitchAdsr + PrKey::bypass, code + PrName::PitchAdsr::bypass, PrValue::PitchAdsr::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::dt, code + PrName::dt1, PrValue::Dt1::min, PrValue::Dt1::max, PrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::dt2, code + PrName::dt2, PrValue::Dt2::min, PrValue::Dt2::max, PrValue::Dt2::initial));

    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Duty::mode, code + PrName::Duty::mode, PrValue::Duty::Mode::min, PrValue::Duty::Mode::max, PrValue::Duty::Mode::initial)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::Duty::preset, code + PrName::Duty::preset, PrValue::Duty::Preset::min, PrValue::Duty::Preset::max, PrValue::Duty::Preset::initial)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Duty::var, code + PrName::Duty::var, PrValue::Duty::Var::min, PrValue::Duty::Var::max, PrValue::Duty::Var::initial)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Duty::inv, code + PrName::Duty::invert, PrValue::Duty::Inv::initial)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Tri::keyTrk, code + PrName::Tri::keyTrack, PrValue::Tri::KeyTrack::initial)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Tri::peak, code + PrName::Tri::peak, PrValue::Tri::Peak::min, PrValue::Tri::Peak::max, PrValue::Tri::Peak::initial)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Tri::freq, code + PrName::Tri::manualFreq, PrValue::Tri::ManualFreq::min, PrValue::Tri::ManualFreq::max, PrValue::Tri::ManualFreq::initial));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::bit, code + PrName::bit, PrValue::Bit::min, PrValue::Bit::max, PrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::rate, code + PrName::rate, PrValue::Rate::min, PrValue::Rate::max, PrValue::Rate::initial)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::HwEnv::enable, code + PrName::HwEnv::enable, PrValue::HwEnv::Enable::initial)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + PrKey::HwEnv::shape, code + PrName::HwEnv::shape, PrValue::HwEnv::Shape::min, PrValue::HwEnv::Shape::max, PrValue::HwEnv::Shape::initial)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::HwEnv::period, code + PrName::HwEnv::period, PrValue::HwEnv::Period::min, PrValue::HwEnv::Period::max, PrValue::HwEnv::Period::initial)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)

    addEnvParameters(layout, code);
    addPitchEnvParameters(layout, code);
}

void SsgProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    // --- SSG Parameters ---
    params.ssg.level = *apvts.getRawParameterValue(code + PrKey::tone);
    params.ssg.noiseLevel = *apvts.getRawParameterValue(code + PrKey::noise);
    params.ssg.noiseFreq = *apvts.getRawParameterValue(code + PrKey::noiseFreq);
    params.ssg.noiseOnNote = (*apvts.getRawParameterValue(code + PrKey::noiseOnNote) > PrValue::boolThread);
    params.ssg.mix = *apvts.getRawParameterValue(code + PrKey::mix);
    params.ssg.waveform = (int)*apvts.getRawParameterValue(code + PrKey::wveform);
    params.ssg.bitDepth = (int)*apvts.getRawParameterValue(code + PrKey::bit);
    params.ssg.rateIndex = (int)*apvts.getRawParameterValue(code + PrKey::rate);

	params.ssg.detune = (int)*apvts.getRawParameterValue(code + PrKey::dt);
    params.ssg.detune2 = (int)*apvts.getRawParameterValue(code + PrKey::dt2);

    params.ssg.dutyMode = (int)*apvts.getRawParameterValue(code + PrKey::Duty::mode);
    params.ssg.dutyPreset = (int)*apvts.getRawParameterValue(code + PrKey::Duty::preset);
    params.ssg.dutyVar = *apvts.getRawParameterValue(code + PrKey::Duty::var);
    params.ssg.dutyInvert = (*apvts.getRawParameterValue(code + PrKey::Duty::inv) > PrValue::boolThread);

    params.ssg.triKeyTrack = (*apvts.getRawParameterValue(code + PrKey::Tri::keyTrk) > PrValue::boolThread);
    params.ssg.triPeak = *apvts.getRawParameterValue(code + PrKey::Tri::peak);
    params.ssg.triFreq = *apvts.getRawParameterValue(code + PrKey::Tri::freq);

    params.ssg.adsr.bypass = (*apvts.getRawParameterValue(code + PrKey::adsr + PrKey::bypass) > PrValue::boolThread);
    params.ssg.adsr.ar = *apvts.getRawParameterValue(code + PrKey::Adsr::ar);
    params.ssg.adsr.dr = *apvts.getRawParameterValue(code + PrKey::Adsr::dr);
    params.ssg.adsr.sl = *apvts.getRawParameterValue(code + PrKey::Adsr::sl);
    params.ssg.adsr.rr = *apvts.getRawParameterValue(code + PrKey::Adsr::rr);

    params.ssg.pitchAdsr.bypass = (*apvts.getRawParameterValue(code + PrKey::pitchAdsr + PrKey::bypass) > PrValue::boolThread);
    params.ssg.pitchAdsr.ar = *apvts.getRawParameterValue(code + PrKey::PitchAdsr::ar);
    params.ssg.pitchAdsr.dr = *apvts.getRawParameterValue(code + PrKey::PitchAdsr::dr);
    params.ssg.pitchAdsr.rr = *apvts.getRawParameterValue(code + PrKey::PitchAdsr::rr);
    params.ssg.pitchAdsr.stl = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::stl);
    params.ssg.pitchAdsr.atl = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::atl);
    params.ssg.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::ssl);
    params.ssg.pitchAdsr.rll = (int)*apvts.getRawParameterValue(code + PrKey::PitchAdsr::rll);

    params.ssg.useHwEnv = (*apvts.getRawParameterValue(code + PrKey::HwEnv::enable) > PrValue::boolThread);
    params.ssg.envShape = (int)*apvts.getRawParameterValue(code + PrKey::HwEnv::shape);
    params.ssg.envPeriod = *apvts.getRawParameterValue(code + PrKey::HwEnv::period);
}
