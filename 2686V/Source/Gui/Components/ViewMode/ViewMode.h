#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

enum class GuiComponentViewModes {
    Top = 0,
    Twin = 1,
    Bottom = 2
};

class GuiComponentViewMode : public GuiBase {
    GuiComponentViewModes viewMode = GuiComponentViewModes::Twin;

    GuiSeparator viewModeSeparator;
    GuiLabel viewModeLabel;
    GuiTextButton viewModeToTopButton;
    GuiTextButton viewModeToTwinButton;
    GuiTextButton viewModeToBottomButton;
public:
    GuiComponentViewMode(const GuiContext& context) :
        GuiBase(context),
        viewModeSeparator(context),
        viewModeLabel(context),
        viewModeToTopButton(context),
        viewModeToTwinButton(context),
        viewModeToBottomButton(context)
    {
    }

    std::function<void(GuiComponentViewModes mode)> onChangeViewMode = nullptr;

    void setupComponent(juce::Component& parent, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
};
