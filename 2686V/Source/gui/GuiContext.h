#pragma once

#include <JuceHeader.h>
#include "../fm/SliderRegMap.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

struct GuiContext
{
	AudioPlugin2686V& audioProcessor;
	AudioPlugin2686VEditor& editor;
	juce::AudioProcessorValueTreeState& apvts;
	SliderRegMap& sliderRegMap;

	GuiContext(AudioPlugin2686V& p, AudioPlugin2686VEditor& e, juce::AudioProcessorValueTreeState& vts, SliderRegMap& srm) :
		audioProcessor(p),
		editor(e),
		apvts(vts),
		sliderRegMap(srm)
	{
	}
};
