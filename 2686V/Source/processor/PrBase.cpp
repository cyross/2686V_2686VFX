// プロセッサ処理の実装

#include "PrBase.h"

#include "../core/PrKeys.h"
#include "../core/PrNames.h"
#include "../core/PrValues.h"

void PrBase::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::ar, prefix + PrName::Adsr::Post::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::dr, prefix + PrName::Adsr::Post::dr, PrValue::Adsr::Dr::min, PrValue::Adsr::Dr::max, PrValue::Adsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::sl, prefix + PrName::Adsr::Post::sl, PrValue::Adsr::Sl::min, PrValue::Adsr::Sl::max, PrValue::Adsr::Sl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::rr, prefix + PrName::Adsr::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
}
