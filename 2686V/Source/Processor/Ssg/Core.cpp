#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void SsgProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = SsgPrKey::prefix;

    // ==========================================
    // SSG Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::tone, code + SsgPrName::tone, SsgPrValue::Tome::min, SsgPrValue::Tome::max, SsgPrValue::Tome::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::noise, code + SsgPrName::Noise::title, SsgPrValue::Noise::min, SsgPrValue::Noise::max, SsgPrValue::Noise::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::noiseFreq, code + SsgPrName::Noise::freq, SsgPrValue::NoiseFreq::min, SsgPrValue::NoiseFreq::max, SsgPrValue::NoiseFreq::initial)); // Noise Frequency Default: 12000Hz (Standard PSG feel)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::noiseOnNote, code + SsgPrName::Noise::noiseOnNote, SsgPrValue::NoiseOnNote::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::mix, code + SsgPrName::toneNoiseMix, SsgPrValue::NoiseMix::min, SsgPrValue::NoiseMix::max, SsgPrValue::NoiseMix::initial)); // デフォルトは 0.5 (Mix)

    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::wveform, code + SsgPrName::waveform, SsgPrValue::waveForm::min, SsgPrValue::waveForm::max, SsgPrValue::waveForm::initial)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::adsr + SsgPrKey::bypass, code + SsgPrName::Adsr::bypass, SsgPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::pitchAdsr + SsgPrKey::bypass, code + SsgPrName::PitchAdsr::bypass, SsgPrValue::PitchAdsr::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::dt, code + SsgPrName::dt1, SsgPrValue::Dt1::min, SsgPrValue::Dt1::max, SsgPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::dt2, code + SsgPrName::dt2, SsgPrValue::Dt2::min, SsgPrValue::Dt2::max, SsgPrValue::Dt2::initial));

    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::Duty::mode, code + SsgPrName::Duty::mode, SsgPrValue::Duty::Mode::min, SsgPrValue::Duty::Mode::max, SsgPrValue::Duty::Mode::initial)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::Duty::preset, code + SsgPrName::Duty::preset, SsgPrValue::Duty::Preset::min, SsgPrValue::Duty::Preset::max, SsgPrValue::Duty::Preset::initial)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Duty::var, code + SsgPrName::Duty::var, SsgPrValue::Duty::Var::min, SsgPrValue::Duty::Var::max, SsgPrValue::Duty::Var::initial)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::Duty::inv, code + SsgPrName::Duty::invert, SsgPrValue::Duty::Inv::initial)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::Tri::keyTrk, code + SsgPrName::Tri::keyTrack, SsgPrValue::Tri::KeyTrack::initial)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Tri::peak, code + SsgPrName::Tri::peak, SsgPrValue::Tri::Peak::min, SsgPrValue::Tri::Peak::max, SsgPrValue::Tri::Peak::initial)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Tri::freq, code + SsgPrName::Tri::manualFreq, SsgPrValue::Tri::ManualFreq::min, SsgPrValue::Tri::ManualFreq::max, SsgPrValue::Tri::ManualFreq::initial));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::bit, code + SsgPrName::bit, SsgPrValue::Bit::min, SsgPrValue::Bit::max, SsgPrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::rate, code + SsgPrName::rate, SsgPrValue::Rate::min, SsgPrValue::Rate::max, SsgPrValue::Rate::initial)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::HwEnv::enable, code + SsgPrName::HwEnv::enable, SsgPrValue::HwEnv::Enable::initial)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::HwEnv::shape, code + SsgPrName::HwEnv::shape, SsgPrValue::HwEnv::Shape::min, SsgPrValue::HwEnv::Shape::max, SsgPrValue::HwEnv::Shape::initial)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::HwEnv::period, code + SsgPrName::HwEnv::period, SsgPrValue::HwEnv::Period::min, SsgPrValue::HwEnv::Period::max, SsgPrValue::HwEnv::Period::initial)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::pmFreq, code + SsgPrName::Lfo::pmFreq, SsgPrValue::Lfo::PmFreq::min, SsgPrValue::Lfo::PmFreq::max, SsgPrValue::Lfo::PmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::amFreq, code + SsgPrName::Lfo::amFreq, SsgPrValue::Lfo::AmFreq::min, SsgPrValue::Lfo::AmFreq::max, SsgPrValue::Lfo::AmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::Lfo::syncDelay, code + SsgPrName::Lfo::syncDelay, SsgPrValue::Lfo::SyncDelay::min, SsgPrValue::Lfo::SyncDelay::max, SsgPrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::amSmoothRatio, code + SsgPrName::Lfo::amSmoothRatio, SsgPrValue::Lfo::AmSmRt::min, SsgPrValue::Lfo::AmSmRt::max, SsgPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::Lfo::pmShape, code + SsgPrName::Lfo::pmShape, SsgPrValue::Lfo::PmShape::min, SsgPrValue::Lfo::PmShape::max, SsgPrValue::Lfo::PmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + SsgPrKey::Lfo::amShape, code + SsgPrName::Lfo::amShape, SsgPrValue::Lfo::AmShape::min, SsgPrValue::Lfo::AmShape::max, SsgPrValue::Lfo::AmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::Lfo::am, code + SsgPrName::Lfo::am, SsgPrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + SsgPrKey::Lfo::pm, code + SsgPrName::Lfo::pm, SsgPrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::pms, code + SsgPrName::Lfo::pms, SsgPrValue::Lfo::Pms::min, SsgPrValue::Lfo::Pms::max, SsgPrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::ams, code + SsgPrName::Lfo::ams, SsgPrValue::Lfo::Ams::min, SsgPrValue::Lfo::Ams::max, SsgPrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::pmd, code + SsgPrName::Lfo::pmd, SsgPrValue::Lfo::Pmd::min, SsgPrValue::Lfo::Pmd::max, SsgPrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + SsgPrKey::Lfo::amd, code + SsgPrName::Lfo::amd, SsgPrValue::Lfo::Amd::min, SsgPrValue::Lfo::Amd::max, SsgPrValue::Lfo::Amd::initial));

    addEnvParameters(layout, code);
    addPitchEnvParameters(layout, code);
}

void SsgProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = SsgPrKey::prefix;

    // --- SSG Parameters ---
    params.ssg.level = *apvts.getRawParameterValue(code + SsgPrKey::tone);
    params.ssg.noiseLevel = *apvts.getRawParameterValue(code + SsgPrKey::noise);
    params.ssg.noiseFreq = *apvts.getRawParameterValue(code + SsgPrKey::noiseFreq);
    params.ssg.noiseOnNote = (*apvts.getRawParameterValue(code + SsgPrKey::noiseOnNote) > SsgPrValue::boolThread);
    params.ssg.mix = *apvts.getRawParameterValue(code + SsgPrKey::mix);
    params.ssg.waveform = (int)*apvts.getRawParameterValue(code + SsgPrKey::wveform);
    params.ssg.bitDepth = (int)*apvts.getRawParameterValue(code + SsgPrKey::bit);
    params.ssg.rateIndex = (int)*apvts.getRawParameterValue(code + SsgPrKey::rate);

	params.ssg.detune = (int)*apvts.getRawParameterValue(code + SsgPrKey::dt);
    params.ssg.detune2 = (int)*apvts.getRawParameterValue(code + SsgPrKey::dt2);

    params.ssg.dutyMode = (int)*apvts.getRawParameterValue(code + SsgPrKey::Duty::mode);
    params.ssg.dutyPreset = (int)*apvts.getRawParameterValue(code + SsgPrKey::Duty::preset);
    params.ssg.dutyVar = *apvts.getRawParameterValue(code + SsgPrKey::Duty::var);
    params.ssg.dutyInvert = (*apvts.getRawParameterValue(code + SsgPrKey::Duty::inv) > SsgPrValue::boolThread);

    params.ssg.triKeyTrack = (*apvts.getRawParameterValue(code + SsgPrKey::Tri::keyTrk) > SsgPrValue::boolThread);
    params.ssg.triPeak = *apvts.getRawParameterValue(code + SsgPrKey::Tri::peak);
    params.ssg.triFreq = *apvts.getRawParameterValue(code + SsgPrKey::Tri::freq);

    params.ssg.adsr.bypass = (*apvts.getRawParameterValue(code + SsgPrKey::adsr + SsgPrKey::bypass) > SsgPrValue::boolThread);
    params.ssg.adsr.ar = *apvts.getRawParameterValue(code + SsgPrKey::Adsr::ar);
    params.ssg.adsr.dr = *apvts.getRawParameterValue(code + SsgPrKey::Adsr::dr);
    params.ssg.adsr.sl = *apvts.getRawParameterValue(code + SsgPrKey::Adsr::sl);
    params.ssg.adsr.rr = *apvts.getRawParameterValue(code + SsgPrKey::Adsr::rr);

    params.ssg.pitchAdsr.bypass = (*apvts.getRawParameterValue(code + SsgPrKey::pitchAdsr + SsgPrKey::bypass) > SsgPrValue::boolThread);
    params.ssg.pitchAdsr.ar = *apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::ar);
    params.ssg.pitchAdsr.dr = *apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::dr);
    params.ssg.pitchAdsr.rr = *apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::rr);
    params.ssg.pitchAdsr.stl = (int)*apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::stl);
    params.ssg.pitchAdsr.atl = (int)*apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::atl);
    params.ssg.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::ssl);
    params.ssg.pitchAdsr.rll = (int)*apvts.getRawParameterValue(code + SsgPrKey::PitchAdsr::rll);

    params.ssg.useHwEnv = (*apvts.getRawParameterValue(code + SsgPrKey::HwEnv::enable) > SsgPrValue::boolThread);
    params.ssg.envShape = (int)*apvts.getRawParameterValue(code + SsgPrKey::HwEnv::shape);
    params.ssg.envPeriod = *apvts.getRawParameterValue(code + SsgPrKey::HwEnv::period);

    params.ssg.lfoPmFreq = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::pmFreq);
    params.ssg.lfoAmFreq = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::amFreq);
    params.ssg.lfoPmWave = (int)*apvts.getRawParameterValue(code + SsgPrKey::Lfo::pmShape);
    params.ssg.lfoAmWave = (int)*apvts.getRawParameterValue(code + SsgPrKey::Lfo::amShape);
    params.ssg.lfoAmSmRt = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::amSmoothRatio);
    params.ssg.lfoAmEnable = (*apvts.getRawParameterValue(code + SsgPrKey::Lfo::am) > SsgPrValue::boolThread);
    params.ssg.lfoPmEnable = (*apvts.getRawParameterValue(code + SsgPrKey::Lfo::pm) > SsgPrValue::boolThread);
    params.ssg.lfoPms = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::pms);
    params.ssg.lfoAms = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::ams);
    params.ssg.lfoPmd = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::pmd);
    params.ssg.lfoAmd = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::amd);
    params.ssg.lfoSyncDelay = *apvts.getRawParameterValue(code + SsgPrKey::Lfo::syncDelay);
}
