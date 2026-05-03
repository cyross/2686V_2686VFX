#include "./Pr.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrNames.h"
#include "../../Core/Const/PrValues.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    juce::String code = PrKey::Prefix::beep;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Beep::level, code + PrName::Beep::Post::level, PrValue::Level::min, PrValue::Level::max, PrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Post::Beep::fix, code + PrName::Beep::Post::fix, PrValue::Fix::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + PrKey::Post::Beep::fixFreq, code + PrName::Beep::Post::fix, PrValue::Fix::Freq::min, PrValue::Fix::Freq::max, PrValue::Fix::Freq::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + PrKey::Innder::adsr + PrKey::Post::bypass, code + PrName::Beep::Post::Adsr::bypass, PrValue::Adsr::Bypass::initial));

    addEnvParameters(layout, code);
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    juce::String code = PrKey::Prefix::beep;

    params.beep.level = *apvts.getRawParameterValue(code + PrKey::Post::Beep::level);
    params.beep.fixedMode = (*apvts.getRawParameterValue(code + PrKey::Post::Beep::fix) > PrValue::boolThread);
    params.beep.fixedFreq = *apvts.getRawParameterValue(code + PrKey::Post::Beep::fixFreq);
    params.beep.bypass = (*apvts.getRawParameterValue(code + PrKey::Innder::adsr + PrKey::Post::bypass) > PrValue::boolThread);
    params.beep.adsr.ar = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::ar);
    params.beep.adsr.dr = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::dr);
    params.beep.adsr.sl = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::sl);
    params.beep.adsr.rr = *apvts.getRawParameterValue(code + PrKey::Post::Adsr::rr);
}
