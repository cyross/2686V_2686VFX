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

class GuiComponentUnison : public GuiBase {
    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiSlider voices;
    GuiSlider detune;
    GuiComponentPitchButtons detuneButtons;
    GuiSlider spread;

public:
    GuiComponentUnison(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        voices(context),
        detune(context),
        detuneButtons(context),
        spread(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void copyParams(CopyUnison& copyObj);
    void pasteParams(CopyUnison& copyObj);
};
