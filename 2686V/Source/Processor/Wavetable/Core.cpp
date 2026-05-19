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
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, WtPrValue::Custom::min, WtPrValue::Custom::max, WtPrValue::Custom::initial));
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
    const juce::String code = WtPrKey::prefix;
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::bit, code + WtPrName::bit, WtPrValue::Bit::min, WtPrValue::Bit::max, WtPrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::rate, code + WtPrName::rate, WtPrValue::Rate::min, WtPrValue::Rate::max, WtPrValue::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64, 2:128, 3:256
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::sampleSize, code + WtPrName::sampleSize, WtPrValue::SammpleSize::min, WtPrValue::SammpleSize::max, WtPrValue::SammpleSize::initial));
    // Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::steps, code + WtPrName::steps, WtPrValue::Steps::min, WtPrValue::Steps::max, WtPrValue::Steps::initial));
    // Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::wave, code + WtPrName::waveform, WtPrValue::WaveForm::min, WtPrValue::WaveForm::max, WtPrValue::WaveForm::initial));

    createCustomWaveLayout(layout, WtPrValue::customSize1, code + WtPrKey::custom32, code + WtPrName::custom32);
    createCustomWaveLayout(layout, WtPrValue::customSize2, code + WtPrKey::custom64, code + WtPrName::custom64);
    createCustomWaveLayout(layout, WtPrValue::customSize3, code + WtPrKey::custom128, code + WtPrName::custom128);
    createCustomWaveLayout(layout, WtPrValue::customSize4, code + WtPrKey::custom256, code + WtPrName::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::Mod::enable, code + WtPrName::Mod::enable, WtPrValue::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Mod::depth, code + WtPrName::Mod::depth, WtPrValue::Mod::Depth::min, WtPrValue::Mod::Depth::max, WtPrValue::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Mod::speed, code + WtPrName::Mod::speed, WtPrValue::Mod::Speed::min, WtPrValue::Mod::Speed::max, WtPrValue::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::level, code + WtPrName::level, WtPrValue::Level::min, WtPrValue::Level::max, WtPrValue::Level::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::dt, code + WtPrName::dt1, WtPrValue::Dt1::min, WtPrValue::Dt1::max, WtPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::dt2, code + WtPrName::dt2, WtPrValue::Dt2::min, WtPrValue::Dt2::max, WtPrValue::Dt2::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::adsr + WtPrKey::bypass, code + WtPrName::Adsr::bypass, WtPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::pitchAdsr + WtPrKey::bypass, code + WtPrName::PitchAdsr::bypass, WtPrValue::PitchAdsr::Bypass::initial));

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::pmFreq, code + WtPrName::Lfo::pmFreq, WtPrValue::Lfo::PmFreq::min, WtPrValue::Lfo::PmFreq::max, WtPrValue::Lfo::PmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::amFreq, code + WtPrName::Lfo::amFreq, WtPrValue::Lfo::AmFreq::min, WtPrValue::Lfo::AmFreq::max, WtPrValue::Lfo::AmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Lfo::syncDelay, code + WtPrName::Lfo::syncDelay, WtPrValue::Lfo::SyncDelay::min, WtPrValue::Lfo::SyncDelay::max, WtPrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::amSmoothRatio, code + WtPrName::Lfo::amSmoothRatio, WtPrValue::Lfo::AmSmRt::min, WtPrValue::Lfo::AmSmRt::max, WtPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Lfo::pmShape, code + WtPrName::Lfo::pmShape, WtPrValue::Lfo::PmShape::min, WtPrValue::Lfo::PmShape::max, WtPrValue::Lfo::PmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Lfo::amShape, code + WtPrName::Lfo::amShape, WtPrValue::Lfo::AmShape::min, WtPrValue::Lfo::AmShape::max, WtPrValue::Lfo::AmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::Lfo::am, code + WtPrName::Lfo::am, WtPrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::Lfo::pm, code + WtPrName::Lfo::pm, WtPrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::pms, code + WtPrName::Lfo::pms, WtPrValue::Lfo::Pms::min, WtPrValue::Lfo::Pms::max, WtPrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::ams, code + WtPrName::Lfo::ams, WtPrValue::Lfo::Ams::min, WtPrValue::Lfo::Ams::max, WtPrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::pmd, code + WtPrName::Lfo::pmd, WtPrValue::Lfo::Pmd::min, WtPrValue::Lfo::Pmd::max, WtPrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::amd, code + WtPrName::Lfo::amd, WtPrValue::Lfo::Amd::min, WtPrValue::Lfo::Amd::max, WtPrValue::Lfo::Amd::initial));

    addEnvParameters(layout, code);
	addPitchEnvParameters(layout, code);
}

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = WtPrKey::prefix;

    params.wt.bitDepth = (int)*apvts.getRawParameterValue(code + WtPrKey::bit);
    params.wt.rateIndex = (int)*apvts.getRawParameterValue(code + WtPrKey::rate);
    params.wt.tableSize = (int)*apvts.getRawParameterValue(code + WtPrKey::sampleSize);
    params.wt.steps = (int)*apvts.getRawParameterValue(code + WtPrKey::steps);
    params.wt.waveform = (int)*apvts.getRawParameterValue(code + WtPrKey::wave);

    processCustomWaveBlock(params.wt.customWave32, apvts, code + WtPrKey::custom32);
    processCustomWaveBlock(params.wt.customWave64, apvts, code + WtPrKey::custom64);
    processCustomWaveBlock(params.wt.customWave128, apvts, code + WtPrKey::custom128);
    processCustomWaveBlock(params.wt.customWave256, apvts, code + WtPrKey::custom256);

    params.wt.modEnable = (*apvts.getRawParameterValue(code + WtPrKey::Mod::enable) > WtPrValue::boolThread);
    params.wt.modDepth = *apvts.getRawParameterValue(code + WtPrKey::Mod::depth);
    params.wt.modSpeed = *apvts.getRawParameterValue(code + WtPrKey::Mod::speed);
    params.wt.level = *apvts.getRawParameterValue(code + WtPrKey::level);

    params.wt.adsr.bypass = (*apvts.getRawParameterValue(code + WtPrKey::adsr + WtPrKey::bypass) > WtPrValue::boolThread);
    params.wt.adsr.stl = *apvts.getRawParameterValue(code + WtPrKey::Adsr::stl);
    params.wt.adsr.ar = *apvts.getRawParameterValue(code + WtPrKey::Adsr::ar);
    params.wt.adsr.dr = *apvts.getRawParameterValue(code + WtPrKey::Adsr::dr);
    params.wt.adsr.sl = *apvts.getRawParameterValue(code + WtPrKey::Adsr::sl);
    params.wt.adsr.rr = *apvts.getRawParameterValue(code + WtPrKey::Adsr::rr);

    params.wt.pitchAdsr.bypass = (*apvts.getRawParameterValue(code + WtPrKey::pitchAdsr + WtPrKey::bypass) > WtPrValue::boolThread);
    params.wt.pitchAdsr.ar = *apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::ar);
    params.wt.pitchAdsr.dr = *apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::dr);
    params.wt.pitchAdsr.rr = *apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::rr);
    params.wt.pitchAdsr.stl = (int)*apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::stl);
    params.wt.pitchAdsr.atl = (int)*apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::atl);
    params.wt.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::ssl);
    params.wt.pitchAdsr.rll = (int)*apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::rll);

    params.wt.detune = (int)*apvts.getRawParameterValue(code + WtPrKey::dt);
    params.wt.detune2 = (int)*apvts.getRawParameterValue(code + WtPrKey::dt2);

    params.wt.lfoPmFreq = *apvts.getRawParameterValue(code + WtPrKey::Lfo::pmFreq);
    params.wt.lfoAmFreq = *apvts.getRawParameterValue(code + WtPrKey::Lfo::amFreq);
    params.wt.lfoPmWave = (int)*apvts.getRawParameterValue(code + WtPrKey::Lfo::pmShape);
    params.wt.lfoAmWave = (int)*apvts.getRawParameterValue(code + WtPrKey::Lfo::amShape);
    params.wt.lfoAmSmRt = *apvts.getRawParameterValue(code + WtPrKey::Lfo::amSmoothRatio);
    params.wt.lfoAmEnable = (*apvts.getRawParameterValue(code + WtPrKey::Lfo::am) > WtPrValue::boolThread);
    params.wt.lfoPmEnable = (*apvts.getRawParameterValue(code + WtPrKey::Lfo::pm) > WtPrValue::boolThread);
    params.wt.lfoPms = *apvts.getRawParameterValue(code + WtPrKey::Lfo::pms);
    params.wt.lfoAms = *apvts.getRawParameterValue(code + WtPrKey::Lfo::ams);
    params.wt.lfoPmd = *apvts.getRawParameterValue(code + WtPrKey::Lfo::pmd);
    params.wt.lfoAmd = *apvts.getRawParameterValue(code + WtPrKey::Lfo::amd);
    params.wt.lfoSyncDelay = *apvts.getRawParameterValue(code + WtPrKey::Lfo::syncDelay);
}
