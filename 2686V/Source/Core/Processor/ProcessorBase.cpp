// プロセッサ処理の実装

#include "./ProcessorBase.h"

#include "./ProcessorKeys.h"
#include "./ProcessorNames.h"
#include "./ProcessorValues.h"

void PrBase::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::ar, prefixName + CorePrName::Adsr::Post::ar, CorePrValue::Adsr::Ar::min, CorePrValue::Adsr::Ar::max, CorePrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::dr, prefixName + CorePrName::Adsr::Post::dr, CorePrValue::Adsr::Dr::min, CorePrValue::Adsr::Dr::max, CorePrValue::Adsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::sl, prefixName + CorePrName::Adsr::Post::sl, CorePrValue::Adsr::Sl::min, CorePrValue::Adsr::Sl::max, CorePrValue::Adsr::Sl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::rr, prefixName + CorePrName::Adsr::Post::rr, CorePrValue::Adsr::Rr::min, CorePrValue::Adsr::Rr::max, CorePrValue::Adsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::stl, prefixName + CorePrKey::Post::Adsr::stl, CorePrValue::Adsr::Stl::min, CorePrValue::Adsr::Stl::max, CorePrValue::Adsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::Adsr::kor, prefixName + CorePrKey::Post::Adsr::kor, CorePrValue::Adsr::Kor::initial));
}

void PrBase::addOpEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::ar, namePrefix + CorePrName::Adsr::Post::ar, CorePrValue::Adsr::Ar::min, CorePrValue::Adsr::Ar::max, CorePrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::dr, namePrefix + CorePrName::Adsr::Post::dr, CorePrValue::Adsr::Dr::min, CorePrValue::Adsr::Dr::max, CorePrValue::Adsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::sl, namePrefix + CorePrName::Adsr::Post::sl, CorePrValue::Adsr::Sl::min, CorePrValue::Adsr::Sl::max, CorePrValue::Adsr::Sl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::rr, namePrefix + CorePrName::Adsr::Post::rr, CorePrValue::Adsr::Rr::min, CorePrValue::Adsr::Rr::max, CorePrValue::Adsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Adsr::stl, namePrefix + CorePrKey::Post::Adsr::stl, CorePrValue::Adsr::Stl::min, CorePrValue::Adsr::Stl::max, CorePrValue::Adsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::Adsr::kor, namePrefix + CorePrKey::Post::Adsr::kor, CorePrValue::Adsr::Kor::initial));
}

void PrBase::addPitchEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::ar, prefixName + CorePrName::PitchAdsr::Post::ar, CorePrValue::PitchAdsr::Ar::min, CorePrValue::PitchAdsr::Ar::max, CorePrValue::PitchAdsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::dr, prefixName + CorePrName::PitchAdsr::Post::dr, CorePrValue::PitchAdsr::Dr::min, CorePrValue::PitchAdsr::Dr::max, CorePrValue::PitchAdsr::Dr::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::PitchAdsr::rr, prefixName + CorePrName::PitchAdsr::Post::rr, CorePrValue::PitchAdsr::Rr::min, CorePrValue::PitchAdsr::Rr::max, CorePrValue::PitchAdsr::Rr::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::stl, prefixName + CorePrName::PitchAdsr::Post::stl, CorePrValue::PitchAdsr::Stl::min, CorePrValue::PitchAdsr::Stl::max, CorePrValue::PitchAdsr::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::atl, prefixName + CorePrName::PitchAdsr::Post::atl, CorePrValue::PitchAdsr::Atl::min, CorePrValue::PitchAdsr::Atl::max, CorePrValue::PitchAdsr::Atl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::ssl, prefixName + CorePrName::PitchAdsr::Post::ssl, CorePrValue::PitchAdsr::Ssl::min, CorePrValue::PitchAdsr::Ssl::max, CorePrValue::PitchAdsr::Ssl::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::PitchAdsr::rll, prefixName + CorePrName::PitchAdsr::Post::rll, CorePrValue::PitchAdsr::Rll::min, CorePrValue::PitchAdsr::Rll::max, CorePrValue::PitchAdsr::Rll::initial));
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

void PrBase::addSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
{
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::steps, prefixName + CorePrName::SsgSwEnv::Post::steps, CorePrValue::SsgSwEnv::Steps::min, CorePrValue::SsgSwEnv::Steps::max, CorePrValue::SsgSwEnv::Steps::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::SsgSwEnv::loop, prefixName + CorePrName::SsgSwEnv::Post::loop, CorePrValue::SsgSwEnv::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::loopTo, prefixName + CorePrName::SsgSwEnv::Post::loopTo, CorePrValue::SsgSwEnv::LoopTo::min, CorePrValue::SsgSwEnv::LoopTo::max, CorePrValue::SsgSwEnv::LoopTo::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::loopCount, prefixName + CorePrName::SsgSwEnv::Post::loopCount, CorePrValue::SsgSwEnv::LoopCount::min, CorePrValue::SsgSwEnv::LoopCount::max, CorePrValue::SsgSwEnv::LoopCount::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::stl, prefixName + CorePrName::SsgSwEnv::Post::stl, CorePrValue::SsgSwEnv::Stl::min, CorePrValue::SsgSwEnv::Stl::max, CorePrValue::SsgSwEnv::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r1, prefixName + CorePrName::SsgSwEnv::Post::r1, CorePrValue::SsgSwEnv::R1::min, CorePrValue::SsgSwEnv::R1::max, CorePrValue::SsgSwEnv::R1::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l1, prefixName + CorePrName::SsgSwEnv::Post::l1, CorePrValue::SsgSwEnv::L1::min, CorePrValue::SsgSwEnv::L1::max, CorePrValue::SsgSwEnv::L1::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r2, prefixName + CorePrName::SsgSwEnv::Post::r2, CorePrValue::SsgSwEnv::R2::min, CorePrValue::SsgSwEnv::R2::max, CorePrValue::SsgSwEnv::R2::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l2, prefixName + CorePrName::SsgSwEnv::Post::l2, CorePrValue::SsgSwEnv::L2::min, CorePrValue::SsgSwEnv::L2::max, CorePrValue::SsgSwEnv::L2::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r3, prefixName + CorePrName::SsgSwEnv::Post::r3, CorePrValue::SsgSwEnv::R3::min, CorePrValue::SsgSwEnv::R3::max, CorePrValue::SsgSwEnv::R3::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l3, prefixName + CorePrName::SsgSwEnv::Post::l3, CorePrValue::SsgSwEnv::L3::min, CorePrValue::SsgSwEnv::L3::max, CorePrValue::SsgSwEnv::L3::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r4, prefixName + CorePrName::SsgSwEnv::Post::r4, CorePrValue::SsgSwEnv::R4::min, CorePrValue::SsgSwEnv::R4::max, CorePrValue::SsgSwEnv::R4::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l4, prefixName + CorePrName::SsgSwEnv::Post::l4, CorePrValue::SsgSwEnv::L4::min, CorePrValue::SsgSwEnv::L4::max, CorePrValue::SsgSwEnv::L4::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r5, prefixName + CorePrName::SsgSwEnv::Post::r5, CorePrValue::SsgSwEnv::R5::min, CorePrValue::SsgSwEnv::R5::max, CorePrValue::SsgSwEnv::R5::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l5, prefixName + CorePrName::SsgSwEnv::Post::l5, CorePrValue::SsgSwEnv::L5::min, CorePrValue::SsgSwEnv::L5::max, CorePrValue::SsgSwEnv::L5::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r6, prefixName + CorePrName::SsgSwEnv::Post::r6, CorePrValue::SsgSwEnv::R6::min, CorePrValue::SsgSwEnv::R6::max, CorePrValue::SsgSwEnv::R6::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l6, prefixName + CorePrName::SsgSwEnv::Post::l6, CorePrValue::SsgSwEnv::L6::min, CorePrValue::SsgSwEnv::L6::max, CorePrValue::SsgSwEnv::L6::initial));
}

void PrBase::addOpSsgSwEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
{
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::steps, namePrefix + CorePrName::SsgSwEnv::Post::steps, CorePrValue::SsgSwEnv::Steps::min, CorePrValue::SsgSwEnv::Steps::max, CorePrValue::SsgSwEnv::Steps::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::SsgSwEnv::loop, namePrefix + CorePrName::SsgSwEnv::Post::loop, CorePrValue::SsgSwEnv::Loop::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::loopTo, namePrefix + CorePrName::SsgSwEnv::Post::loopTo, CorePrValue::SsgSwEnv::LoopTo::min, CorePrValue::SsgSwEnv::LoopTo::max, CorePrValue::SsgSwEnv::LoopTo::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::SsgSwEnv::loopCount, namePrefix + CorePrName::SsgSwEnv::Post::loopCount, CorePrValue::SsgSwEnv::LoopCount::min, CorePrValue::SsgSwEnv::LoopCount::max, CorePrValue::SsgSwEnv::LoopCount::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::stl, namePrefix + CorePrName::SsgSwEnv::Post::stl, CorePrValue::SsgSwEnv::Stl::min, CorePrValue::SsgSwEnv::Stl::max, CorePrValue::SsgSwEnv::Stl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r1, namePrefix + CorePrName::SsgSwEnv::Post::r1, CorePrValue::SsgSwEnv::R1::min, CorePrValue::SsgSwEnv::R1::max, CorePrValue::SsgSwEnv::R1::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l1, namePrefix + CorePrName::SsgSwEnv::Post::l1, CorePrValue::SsgSwEnv::L1::min, CorePrValue::SsgSwEnv::L1::max, CorePrValue::SsgSwEnv::L1::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r2, namePrefix + CorePrName::SsgSwEnv::Post::r2, CorePrValue::SsgSwEnv::R2::min, CorePrValue::SsgSwEnv::R2::max, CorePrValue::SsgSwEnv::R2::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l2, namePrefix + CorePrName::SsgSwEnv::Post::l2, CorePrValue::SsgSwEnv::L2::min, CorePrValue::SsgSwEnv::L2::max, CorePrValue::SsgSwEnv::L2::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r3, namePrefix + CorePrName::SsgSwEnv::Post::r3, CorePrValue::SsgSwEnv::R3::min, CorePrValue::SsgSwEnv::R3::max, CorePrValue::SsgSwEnv::R3::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l3, namePrefix + CorePrName::SsgSwEnv::Post::l3, CorePrValue::SsgSwEnv::L3::min, CorePrValue::SsgSwEnv::L3::max, CorePrValue::SsgSwEnv::L3::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r4, namePrefix + CorePrName::SsgSwEnv::Post::r4, CorePrValue::SsgSwEnv::R4::min, CorePrValue::SsgSwEnv::R4::max, CorePrValue::SsgSwEnv::R4::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l4, namePrefix + CorePrName::SsgSwEnv::Post::l4, CorePrValue::SsgSwEnv::L4::min, CorePrValue::SsgSwEnv::L4::max, CorePrValue::SsgSwEnv::L4::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r5, namePrefix + CorePrName::SsgSwEnv::Post::r5, CorePrValue::SsgSwEnv::R5::min, CorePrValue::SsgSwEnv::R5::max, CorePrValue::SsgSwEnv::R5::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l5, namePrefix + CorePrName::SsgSwEnv::Post::l5, CorePrValue::SsgSwEnv::L5::min, CorePrValue::SsgSwEnv::L5::max, CorePrValue::SsgSwEnv::L5::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::r6, namePrefix + CorePrName::SsgSwEnv::Post::r6, CorePrValue::SsgSwEnv::R6::min, CorePrValue::SsgSwEnv::R6::max, CorePrValue::SsgSwEnv::R6::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::SsgSwEnv::l6, namePrefix + CorePrName::SsgSwEnv::Post::l6, CorePrValue::SsgSwEnv::L6::min, CorePrValue::SsgSwEnv::L6::max, CorePrValue::SsgSwEnv::L6::initial));
}

void PrBase::addOpzx7LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::pmFreq, prefixName + CorePrName::Lfo::Post::pmFreq, CorePrValue::Lfo::PmFreq::min, CorePrValue::Lfo::PmFreq::max, CorePrValue::Lfo::PmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::amFreq, prefixName + CorePrName::Lfo::Post::amFreq, CorePrValue::Lfo::AmFreq::min, CorePrValue::Lfo::AmFreq::max, CorePrValue::Lfo::AmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::pmSyncDelay, prefixName + CorePrName::Lfo::Post::pmSyncDelay, CorePrValue::Lfo::SyncDelay::min, CorePrValue::Lfo::SyncDelay::max, CorePrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::amSyncDelay, prefixName + CorePrName::Lfo::Post::amSyncDelay, CorePrValue::Lfo::SyncDelay::min, CorePrValue::Lfo::SyncDelay::max, CorePrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::amSmoothRatio, prefixName + CorePrName::Lfo::Post::amSmoothRatio, CorePrValue::Lfo::AmSmRt::min, CorePrValue::Lfo::AmSmRt::max, CorePrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::pgShape, prefixName + CorePrName::Lfo::Post::pgShape, CorePrValue::Lfo::PmShape::min, CorePrValue::Lfo::PmShape::max, CorePrValue::Lfo::PmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::egShape, prefixName + CorePrName::Lfo::Post::egShape, CorePrValue::Lfo::AmShape::min, CorePrValue::Lfo::AmShape::max, CorePrValue::Lfo::AmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::Lfo::am, prefixName + CorePrName::Lfo::Post::am, CorePrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::Lfo::pm, prefixName + CorePrName::Lfo::Post::pm, CorePrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::pms, prefixName + CorePrName::Lfo::Post::pms, CorePrValue::Lfo::Pms::min, CorePrValue::Lfo::Pms::max, CorePrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::ams, prefixName + CorePrName::Lfo::Post::ams, CorePrValue::Lfo::Ams::min, CorePrValue::Lfo::Ams::max, CorePrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::pmd, prefixName + CorePrName::Lfo::Post::pmd, CorePrValue::Lfo::Pmd::min, CorePrValue::Lfo::Pmd::max, CorePrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::amd, prefixName + CorePrName::Lfo::Post::amd, CorePrValue::Lfo::Amd::min, CorePrValue::Lfo::Amd::max, CorePrValue::Lfo::Amd::initial));
}

void PrBase::addOpOpzx7LfoParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& namePrefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::pmFreq, namePrefix + CorePrName::Lfo::Post::pmFreq, CorePrValue::Lfo::PmFreq::min, CorePrValue::Lfo::PmFreq::max, CorePrValue::Lfo::PmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::amFreq, namePrefix + CorePrName::Lfo::Post::amFreq, CorePrValue::Lfo::PmFreq::min, CorePrValue::Lfo::PmFreq::max, CorePrValue::Lfo::PmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::pmSyncDelay, namePrefix + CorePrName::Lfo::Post::pmSyncDelay, CorePrValue::Lfo::SyncDelay::min, CorePrValue::Lfo::SyncDelay::max, CorePrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::amSyncDelay, namePrefix + CorePrName::Lfo::Post::amSyncDelay, CorePrValue::Lfo::SyncDelay::min, CorePrValue::Lfo::SyncDelay::max, CorePrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::amSmoothRatio, namePrefix + CorePrName::Lfo::Post::amSmoothRatio, CorePrValue::Lfo::AmSmRt::min, CorePrValue::Lfo::AmSmRt::max, CorePrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::pgShape, namePrefix + CorePrName::Lfo::Post::pgShape, CorePrValue::Lfo::PmShape::min, CorePrValue::Lfo::PmShape::max, CorePrValue::Lfo::PmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(prefix + CorePrKey::Post::Lfo::egShape, namePrefix + CorePrName::Lfo::Post::egShape, CorePrValue::Lfo::AmShape::min, CorePrValue::Lfo::AmShape::max, CorePrValue::Lfo::AmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::Lfo::am, namePrefix + CorePrName::Lfo::Post::am, CorePrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(prefix + CorePrKey::Post::Lfo::pm, namePrefix + CorePrName::Lfo::Post::pm, CorePrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::pms, namePrefix + CorePrName::Lfo::Post::pms, CorePrValue::Lfo::Pms::min, CorePrValue::Lfo::Pms::max, CorePrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::ams, namePrefix + CorePrName::Lfo::Post::ams, CorePrValue::Lfo::Ams::min, CorePrValue::Lfo::Ams::max, CorePrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::pmd, namePrefix + CorePrName::Lfo::Post::pmd, CorePrValue::Lfo::Pmd::min, CorePrValue::Lfo::Pmd::max, CorePrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + CorePrKey::Post::Lfo::amd, namePrefix + CorePrName::Lfo::Post::amd, CorePrValue::Lfo::Amd::min, CorePrValue::Lfo::Amd::max, CorePrValue::Lfo::Amd::initial));
}
