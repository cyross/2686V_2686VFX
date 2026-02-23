#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <span>

#include "GuiContext.h"

class GuiBase: public juce::Component
{
public:
	GuiBase(const GuiContext& context) : ctx(context) {}

    virtual void setup() {};
    virtual void layout(juce::Rectangle<int> content) {};
protected:
	// GuiOpnaなどでも使うので、using宣言でエイリアスを作っておく
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    GuiContext ctx;
};
