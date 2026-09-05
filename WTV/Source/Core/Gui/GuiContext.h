#pragma once

#include <JuceHeader.h>

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

struct GuiContext
{
	AudioPlugin2686V& audioProcessor;
	AudioPlugin2686VEditor& editor;
	juce::AudioProcessorValueTreeState& apvts;

	GuiContext(AudioPlugin2686V& p, AudioPlugin2686VEditor& e, juce::AudioProcessorValueTreeState& vts) :
		audioProcessor(p),
		editor(e),
		apvts(vts)
	{
	}
};
