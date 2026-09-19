#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"

class GuiComponentPresetName : public GuiBase {
    // プリセット名ラベル
    GuiLabel presetNameLabel;
    NormalSeparator presetNameSeparator;
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
