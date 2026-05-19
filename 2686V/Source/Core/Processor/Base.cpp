// プロセッサ処理の実装

#include "./Base.h"

#include "./Keys.h"
#include "./Names.h"
#include "./Values.h"

void PrBase::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::ar, prefix + CorePrName::Adsr::Post::ar, CorePrValue::Adsr::Ar::min, CorePrValue::Adsr::Ar::max, CorePrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::dr, prefix + CorePrName::Adsr::Post::dr, CorePrValue::Adsr::Dr::min, CorePrValue::Adsr::Dr::max, CorePrValue::Adsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::sl, prefix + CorePrName::Adsr::Post::sl, CorePrValue::Adsr::Sl::min, CorePrValue::Adsr::Sl::max, CorePrValue::Adsr::Sl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::rr, prefix + CorePrName::Adsr::Post::rr, CorePrValue::Adsr::Rr::min, CorePrValue::Adsr::Rr::max, CorePrValue::Adsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::stl, prefix + CorePrKey::Post::Adsr::stl, CorePrValue::Adsr::Stl::min, CorePrValue::Adsr::Stl::max, CorePrValue::Adsr::Stl::initial));
}

void PrBase::addPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::ar, prefix + CorePrName::PitchAdsr::Post::ar, CorePrValue::PitchAdsr::Ar::min, CorePrValue::PitchAdsr::Ar::max, CorePrValue::PitchAdsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::dr, prefix + CorePrName::PitchAdsr::Post::dr, CorePrValue::PitchAdsr::Dr::min, CorePrValue::PitchAdsr::Dr::max, CorePrValue::PitchAdsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::rr, prefix + CorePrName::PitchAdsr::Post::rr, CorePrValue::PitchAdsr::Rr::min, CorePrValue::PitchAdsr::Rr::max, CorePrValue::PitchAdsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::stl, prefix + CorePrName::PitchAdsr::Post::stl, CorePrValue::PitchAdsr::Stl::min, CorePrValue::PitchAdsr::Stl::max, CorePrValue::PitchAdsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::atl, prefix + CorePrName::PitchAdsr::Post::atl, CorePrValue::PitchAdsr::Atl::min, CorePrValue::PitchAdsr::Atl::max, CorePrValue::PitchAdsr::Atl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::ssl, prefix + CorePrName::PitchAdsr::Post::ssl, CorePrValue::PitchAdsr::Ssl::min, CorePrValue::PitchAdsr::Ssl::max, CorePrValue::PitchAdsr::Ssl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::rll, prefix + CorePrName::PitchAdsr::Post::rll, CorePrValue::PitchAdsr::Rll::min, CorePrValue::PitchAdsr::Rll::max, CorePrValue::PitchAdsr::Rll::initial));
}

void PrBase::addOpPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::ar, namePrefix + CorePrName::PitchAdsr::Post::ar, CorePrValue::PitchAdsr::Ar::min, CorePrValue::PitchAdsr::Ar::max, CorePrValue::PitchAdsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::dr, namePrefix + CorePrName::PitchAdsr::Post::dr, CorePrValue::PitchAdsr::Dr::min, CorePrValue::PitchAdsr::Dr::max, CorePrValue::PitchAdsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::rr, namePrefix + CorePrName::PitchAdsr::Post::rr, CorePrValue::PitchAdsr::Rr::min, CorePrValue::PitchAdsr::Rr::max, CorePrValue::PitchAdsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::stl, namePrefix + CorePrName::PitchAdsr::Post::stl, CorePrValue::PitchAdsr::Stl::min, CorePrValue::PitchAdsr::Stl::max, CorePrValue::PitchAdsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::atl, namePrefix + CorePrName::PitchAdsr::Post::atl, CorePrValue::PitchAdsr::Atl::min, CorePrValue::PitchAdsr::Atl::max, CorePrValue::PitchAdsr::Atl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::ssl, namePrefix + CorePrName::PitchAdsr::Post::ssl, CorePrValue::PitchAdsr::Ssl::min, CorePrValue::PitchAdsr::Ssl::max, CorePrValue::PitchAdsr::Ssl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::rll, namePrefix + CorePrName::PitchAdsr::Post::rll, CorePrValue::PitchAdsr::Rll::min, CorePrValue::PitchAdsr::Rll::max, CorePrValue::PitchAdsr::Rll::initial));
}

void PrBase::addSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::steps, prefix + CorePrName::SsgSwEnv::Post::steps, CorePrValue::SsgSwEnv::Steps::min, CorePrValue::SsgSwEnv::Steps::max, CorePrValue::SsgSwEnv::Steps::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::stl, prefix + CorePrName::SsgSwEnv::Post::stl, CorePrValue::SsgSwEnv::Stl::min, CorePrValue::SsgSwEnv::Stl::max, CorePrValue::SsgSwEnv::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r0, prefix + CorePrName::SsgSwEnv::Post::r0, CorePrValue::SsgSwEnv::R0::min, CorePrValue::SsgSwEnv::R0::max, CorePrValue::SsgSwEnv::R0::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l0, prefix + CorePrName::SsgSwEnv::Post::l0, CorePrValue::SsgSwEnv::L0::min, CorePrValue::SsgSwEnv::L0::max, CorePrValue::SsgSwEnv::L0::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r1, prefix + CorePrName::SsgSwEnv::Post::r1, CorePrValue::SsgSwEnv::R1::min, CorePrValue::SsgSwEnv::R1::max, CorePrValue::SsgSwEnv::R1::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l1, prefix + CorePrName::SsgSwEnv::Post::l1, CorePrValue::SsgSwEnv::L1::min, CorePrValue::SsgSwEnv::L1::max, CorePrValue::SsgSwEnv::L1::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r2, prefix + CorePrName::SsgSwEnv::Post::r2, CorePrValue::SsgSwEnv::R2::min, CorePrValue::SsgSwEnv::R2::max, CorePrValue::SsgSwEnv::R2::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l2, prefix + CorePrName::SsgSwEnv::Post::l2, CorePrValue::SsgSwEnv::L2::min, CorePrValue::SsgSwEnv::L2::max, CorePrValue::SsgSwEnv::L2::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r3, prefix + CorePrName::SsgSwEnv::Post::r3, CorePrValue::SsgSwEnv::R3::min, CorePrValue::SsgSwEnv::R3::max, CorePrValue::SsgSwEnv::R3::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l3, prefix + CorePrName::SsgSwEnv::Post::l3, CorePrValue::SsgSwEnv::L3::min, CorePrValue::SsgSwEnv::L3::max, CorePrValue::SsgSwEnv::L3::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r4, prefix + CorePrName::SsgSwEnv::Post::r4, CorePrValue::SsgSwEnv::R4::min, CorePrValue::SsgSwEnv::R4::max, CorePrValue::SsgSwEnv::R4::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l4, prefix + CorePrName::SsgSwEnv::Post::l4, CorePrValue::SsgSwEnv::L4::min, CorePrValue::SsgSwEnv::L4::max, CorePrValue::SsgSwEnv::L4::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r5, prefix + CorePrName::SsgSwEnv::Post::r5, CorePrValue::SsgSwEnv::R5::min, CorePrValue::SsgSwEnv::R5::max, CorePrValue::SsgSwEnv::R5::initial));
}
