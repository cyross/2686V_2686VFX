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

    addEnvParameters(layout, code);
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = BeepPrKey::prefix;

    params.beep.level = *apvts.getRawParameterValue(code + BeepPrKey::level);
    params.beep.fixedMode = (*apvts.getRawParameterValue(code + BeepPrKey::fix) > BeepPrValue::boolThread);
    params.beep.fixedFreq = *apvts.getRawParameterValue(code + BeepPrKey::fixFreq);
    params.beep.adsr.bypass = (*apvts.getRawParameterValue(code + BeepPrKey::adsr + BeepPrKey::bypass) > BeepPrValue::boolThread);
    params.beep.adsr.ar = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::ar);
    params.beep.adsr.dr = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::dr);
    params.beep.adsr.sl = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::sl);
    params.beep.adsr.rr = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::rr);
}
