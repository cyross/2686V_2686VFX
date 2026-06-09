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

class GuiComponentMidi : public GuiBase {
    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiToggleButton monoMode;
    GuiToggleButton useVelocity;
    GuiSlider fixedVelocity;
    GuiToggleButton pitchResetOnLegato;
    GuiSeparator resetSeparator;
    GuiTextButton resetButton;
public:
    GuiComponentMidi(const GuiContext& context) :
        GuiBase(context),
        cat(context),
		monoMode(context),
        useVelocity(context),
        fixedVelocity(context),
        pitchResetOnLegato(context),
        resetSeparator(context),
        resetButton(context)
    {
    }

    void setupComponent(juce::Component& parent, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnables(bool enabled);
};
