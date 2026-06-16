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
#include "../../../Gui/Curve/GuiCurve.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"
#include "../PitchButtons/PitchButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

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
    void copyParams(CopyDetuneOpzx7& copyObj);
    void pasteParams(CopyDetuneOpzx7& copyObj);
};
