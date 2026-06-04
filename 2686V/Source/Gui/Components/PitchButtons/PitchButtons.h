#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "../../../Core/Const/Global.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Curve/Core.h"
#include "../../../Advanced/Curve/Core.h"

class GuiComponentPitchButtons : public GuiBase {
    GuiTextButton minus1200;
    GuiTextButton minus100;
    GuiTextButton pm0;
    GuiTextButton plus100;
    GuiTextButton plus1200;

public:
    GuiComponentPitchButtons(const GuiContext& context) :
        GuiBase(context),
        minus1200(context),
        minus100(context),
        pm0(context),
        plus100(context),
        plus1200(context)
    {
    }

    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font = nullopt);
    void layoutComponent(juce::Rectangle<int>& rect, int height = 15);
    void layoutComponentRow(juce::Rectangle<int>& rect, int height = 14);
    void setVisibles(bool visible);
    void setEnables(bool enabled);
};
