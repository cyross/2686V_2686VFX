#pragma once

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

class GuiComponentPresetName : public GuiBase {
    // プリセット名ラベル
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;
public:
    GuiComponentPresetName(const GuiContext& context) :
        GuiBase(context),
        presetNameLabel(context),
        presetNameSeparator(context)
    {
    }
    void setupComponent(juce::Component& parent, int& tabOrder, const juce::String& name);
    void layoutComponent(juce::Rectangle<int>& rect);
    void updatePresetName(const juce::String& name);
};
