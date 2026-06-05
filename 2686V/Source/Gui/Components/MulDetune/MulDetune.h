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
#include "../PitchButtons/PitchButtons.h"

class GuiComponentMulDetune : public GuiBase {
    // MULTIPLE/DETUNE
    GuiCategoryLabel cat;
    GuiComboBox mul;
    GuiSlider mulRatio;
    GuiComboBox dt1;
    GuiSlider dt2;
    GuiSlider dt3;
    GuiComponentPitchButtons dt3Buttons;

public:
    GuiComponentMulDetune(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        mul(context),
        mulRatio(context),
        dt1(context),
        dt2(context),
        dt3(context),
        dt3Buttons(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, bool isHw = false);
    void layoutComponent(juce::Rectangle<int>& rect);
};
