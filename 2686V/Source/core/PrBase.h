#pragma once

#include <JuceHeader.h>
#include "../fm/OpConstants.h"
#include "../fm/OpValueRange.h"
#include "../gui/LabelConstants.h"

class PrBase
{
protected:
    void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
};
