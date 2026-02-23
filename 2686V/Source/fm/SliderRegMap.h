#pragma once

#include <JuceHeader.h>
#include <map>

#include "RegisterType.h"

class SliderRegMap
{
    std::map<juce::Component*, RegisterType> sliderRegMap;
public:
    int count(juce::Slider* slider);
    RegisterType& operator[](juce::Component* slider)
    {
        return sliderRegMap[slider];
	}

    void set(juce::Component* slider, RegisterType type)
    {
        sliderRegMap[slider] = type;
	}
};
