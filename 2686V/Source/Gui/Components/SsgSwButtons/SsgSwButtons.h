#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Curve/GuiCurve.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"

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
