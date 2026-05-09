// プロセッサ処理の実装

#include "./PrBase.h"

#include "../Const/PrKeys.h"
#include "../Const/PrNames.h"
#include "../Const/PrValues.h"

void PrBase::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::ar, prefix + PrName::Adsr::Post::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::dr, prefix + PrName::Adsr::Post::dr, PrValue::Adsr::Dr::min, PrValue::Adsr::Dr::max, PrValue::Adsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::sl, prefix + PrName::Adsr::Post::sl, PrValue::Adsr::Sl::min, PrValue::Adsr::Sl::max, PrValue::Adsr::Sl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::rr, prefix + PrName::Adsr::Post::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
}

void PrBase::addPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::PitchAdsr::ar, prefix + PrName::PitchAdsr::Post::ar, PrValue::PitchAdsr::Ar::min, PrValue::PitchAdsr::Ar::max, PrValue::PitchAdsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::PitchAdsr::dr, prefix + PrName::PitchAdsr::Post::dr, PrValue::PitchAdsr::Dr::min, PrValue::PitchAdsr::Dr::max, PrValue::PitchAdsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::PitchAdsr::rr, prefix + PrName::PitchAdsr::Post::rr, PrValue::PitchAdsr::Rr::min, PrValue::PitchAdsr::Rr::max, PrValue::PitchAdsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::stl, prefix + PrName::PitchAdsr::Post::stl, PrValue::PitchAdsr::Stl::min, PrValue::PitchAdsr::Stl::max, PrValue::PitchAdsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::atl, prefix + PrName::PitchAdsr::Post::atl, PrValue::PitchAdsr::Atl::min, PrValue::PitchAdsr::Atl::max, PrValue::PitchAdsr::Atl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::ssl, prefix + PrName::PitchAdsr::Post::ssl, PrValue::PitchAdsr::Ssl::min, PrValue::PitchAdsr::Ssl::max, PrValue::PitchAdsr::Ssl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::rll, prefix + PrName::PitchAdsr::Post::rll, PrValue::PitchAdsr::Rll::min, PrValue::PitchAdsr::Rll::max, PrValue::PitchAdsr::Rll::initial));
}

void PrBase::addOpPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::PitchAdsr::ar, namePrefix + PrName::PitchAdsr::Post::ar, PrValue::PitchAdsr::Ar::min, PrValue::PitchAdsr::Ar::max, PrValue::PitchAdsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::PitchAdsr::dr, namePrefix + PrName::PitchAdsr::Post::dr, PrValue::PitchAdsr::Dr::min, PrValue::PitchAdsr::Dr::max, PrValue::PitchAdsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::PitchAdsr::rr, namePrefix + PrName::PitchAdsr::Post::rr, PrValue::PitchAdsr::Rr::min, PrValue::PitchAdsr::Rr::max, PrValue::PitchAdsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::stl, namePrefix + PrName::PitchAdsr::Post::stl, PrValue::PitchAdsr::Stl::min, PrValue::PitchAdsr::Stl::max, PrValue::PitchAdsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::atl, namePrefix + PrName::PitchAdsr::Post::atl, PrValue::PitchAdsr::Atl::min, PrValue::PitchAdsr::Atl::max, PrValue::PitchAdsr::Atl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::ssl, namePrefix + PrName::PitchAdsr::Post::ssl, PrValue::PitchAdsr::Ssl::min, PrValue::PitchAdsr::Ssl::max, PrValue::PitchAdsr::Ssl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + PrKey::Post::PitchAdsr::rll, namePrefix + PrName::PitchAdsr::Post::rll, PrValue::PitchAdsr::Rll::min, PrValue::PitchAdsr::Rll::max, PrValue::PitchAdsr::Rll::initial));
}
