#include "PrBeep.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    juce::String code = PrKey::Prefix::beep;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Beep::level, code + PrName::Beep::Post::level, PrValue::Beep::Level::min, PrValue::Beep::Level::max, PrValue::Beep::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Beep::fix, code + PrName::Beep::Post::fix, PrValue::Beep::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Beep::fixFreq, code + PrName::Beep::Post::fix, PrValue::Beep::FixFreq::min, PrValue::Beep::FixFreq::max, PrValue::Beep::FixFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Innder::adsr + PrKey::Post::bypass, code + PrName::Beep::Post::Adsr::bypass, PrValue::Beep::Adsr::Bypass::initial));

    addEnvParameters(layout, code);
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    juce::String code = PrKey::Prefix::beep;

    params.beepLevel = *apvts.getRawParameterValue(code + PrKey::Post::Beep::level);
    params.beepFixedMode = (*apvts.getRawParameterValue(code + PrKey::Post::Beep::fix) > PrValue::boolThread);
    params.beepFixedFreq = *apvts.getRawParameterValue(code + PrKey::Post::Beep::fixFreq);
    params.beepAdsrBypass = (*apvts.getRawParameterValue(code + PrKey::Innder::adsr + PrKey::Post::bypass) > PrValue::boolThread);
    params.beepAdsr.a = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::ar);
    params.beepAdsr.d = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::dr);
    params.beepAdsr.s = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::sl);
    params.beepAdsr.r = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::rr);
}
