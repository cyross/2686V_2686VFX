#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = PrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::level, code + PrName::level, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::fix, code + PrName::fix, PrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::fixFreq, code + PrName::fixFreq, PrValue::FixFreq::min, PrValue::FixFreq::max, PrValue::FixFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::adsr + PrKey::bypass, code + PrName::Adsr::bypass, PrValue::Adsr::Bypass::initial));

    addEnvParameters(layout, code);
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = PrKey::prefix;

    params.beep.level = *apvts.getRawParameterValue(code + PrKey::level);
    params.beep.fixedMode = (*apvts.getRawParameterValue(code + PrKey::fix) > PrValue::boolThread);
    params.beep.fixedFreq = *apvts.getRawParameterValue(code + PrKey::fixFreq);
    params.beep.adsr.bypass = (*apvts.getRawParameterValue(code + PrKey::adsr + PrKey::bypass) > PrValue::boolThread);
    params.beep.adsr.ar = *apvts.getRawParameterValue(code + PrKey::Adsr::ar);
    params.beep.adsr.dr = *apvts.getRawParameterValue(code + PrKey::Adsr::dr);
    params.beep.adsr.sl = *apvts.getRawParameterValue(code + PrKey::Adsr::sl);
    params.beep.adsr.rr = *apvts.getRawParameterValue(code + PrKey::Adsr::rr);
}
