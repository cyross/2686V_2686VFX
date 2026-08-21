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
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"

class GuiComponentMidi : public GuiBase {
    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiToggleButton monoMode;
    NormalSeparator separator1;
    GuiToggleButton useVelocity;
    GuiSlider fixedVelocity;
    NormalSeparator separator2;
    GuiToggleButton pitchResetOnLegato;
    NormalSeparator resetSeparator;
    GuiTextButton monoButton;
    GuiTextButton polyButton;
public:
    GuiComponentMidi(const GuiContext& context) :
        GuiBase(context),
        cat(context),
		monoMode(context),
        separator1(context),
        useVelocity(context),
        fixedVelocity(context),
        separator2(context),
        pitchResetOnLegato(context),
        resetSeparator(context),
        monoButton(context),
        polyButton(context)
    {
    }

    void setupComponent(juce::Component& parent, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnables(bool enabled);
};
