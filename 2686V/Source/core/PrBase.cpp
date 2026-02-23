#include "PrBase.h"

void PrBase::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, prefix + opPostArLabel, opArMin, opArMax, opArDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, prefix + opPostDrLabel, opDrMin, opDrMax, opDrDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, prefix + opPostSlLabel, opSlMin, opSlMax, opSlDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, prefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
}
