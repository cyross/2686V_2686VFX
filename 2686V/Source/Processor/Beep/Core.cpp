#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = BeepPrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + BeepPrKey::level, code + BeepPrName::level, BeepPrValue::Level::min, BeepPrValue::Level::max, BeepPrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::fix, code + BeepPrName::fix, BeepPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + BeepPrKey::fixFreq, code + BeepPrName::fixFreq, BeepPrValue::FixFreq::min, BeepPrValue::FixFreq::max, BeepPrValue::FixFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::adsr + BeepPrKey::bypass, code + BeepPrName::Adsr::bypass, BeepPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::pitchAdsr + BeepPrKey::bypass, code + BeepPrName::PitchAdsr::bypass, BeepPrValue::PitchAdsr::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::dt, code + BeepPrName::dt1, BeepPrValue::Dt1::min, BeepPrValue::Dt1::max, BeepPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::dt2, code + BeepPrName::dt2, BeepPrValue::Dt2::min, BeepPrValue::Dt2::max, BeepPrValue::Dt2::initial));

    addEnvParameters(layout, code);
    addPitchEnvParameters(layout, code);
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = BeepPrKey::prefix;

    params.beep.level = *apvts.getRawParameterValue(code + BeepPrKey::level);
    params.beep.fixedMode = (*apvts.getRawParameterValue(code + BeepPrKey::fix) > BeepPrValue::boolThread);
    params.beep.fixedFreq = *apvts.getRawParameterValue(code + BeepPrKey::fixFreq);
    params.beep.adsr.bypass = (*apvts.getRawParameterValue(code + BeepPrKey::adsr + BeepPrKey::bypass) > BeepPrValue::boolThread);
    params.beep.adsr.stl = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::stl);
    params.beep.adsr.ar = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::ar);
    params.beep.adsr.dr = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::dr);
    params.beep.adsr.sl = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::sl);
    params.beep.adsr.rr = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::rr);

    params.beep.pitchAdsr.bypass = (*apvts.getRawParameterValue(code + BeepPrKey::pitchAdsr + BeepPrKey::bypass) > BeepPrValue::boolThread);
    params.beep.pitchAdsr.ar = *apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::ar);
    params.beep.pitchAdsr.dr = *apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::dr);
    params.beep.pitchAdsr.rr = *apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::rr);
    params.beep.pitchAdsr.stl = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::stl);
    params.beep.pitchAdsr.atl = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::atl);
    params.beep.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::ssl);
    params.beep.pitchAdsr.rll = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::rll);

    params.beep.detune = (int)*apvts.getRawParameterValue(code + BeepPrKey::dt);
    params.beep.detune2 = (int)*apvts.getRawParameterValue(code + BeepPrKey::dt2);
}
