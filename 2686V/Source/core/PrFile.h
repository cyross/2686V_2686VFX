#pragma once

#include <JuceHeader.h>

class GuiFile
{
	juce::File file;
public:
	void setFile(const juce::File& f) { file = f; }
	juce::File& getFile() { return file; }
	void initPreset();
	void loadPreset(const juce::File& file);
	void savePreset(const juce::File& file);
};
