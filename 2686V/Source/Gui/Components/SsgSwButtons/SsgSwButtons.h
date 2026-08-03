#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"

class GuiComponentSsgSwButtons : public GuiBase {
    GuiTextButton minus001;
    GuiTextButton minus01;
    GuiTextButton pm0;
    GuiTextButton pm1;
    GuiTextButton plus01;
    GuiTextButton plus001;

public:
    GuiComponentSsgSwButtons(const GuiContext& context) :
        GuiBase(context),
        minus001(context),
        minus01(context),
        pm0(context),
        pm1(context),
        plus01(context),
        plus001(context)
    {
    }

    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font = nullopt);
    void layoutComponent(juce::Rectangle<int>& rect, int height = 15);
    void layoutComponentRow(juce::Rectangle<int>& rect, int height = 14);
    void setVisibles(bool visible);
    void setEnables(bool enabled);
};
