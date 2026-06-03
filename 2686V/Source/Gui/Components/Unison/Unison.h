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

class GuiComponentUnison : public GuiBase {
    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiSlider voices;
    GuiSlider detune;
    GuiTextButton detuneMinus1200;
    GuiTextButton detuneMinus100;
    GuiTextButton detune0;
    GuiTextButton detunePlus100;
    GuiTextButton detunePlus120;
    GuiSlider spread;

public:
    GuiComponentUnison(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        voices(context),
        detune(context),
        detuneMinus1200(context),
        detuneMinus100(context),
        detune0(context),
        detunePlus100(context),
        detunePlus120(context),
        spread(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
};
