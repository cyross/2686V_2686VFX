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

class GuiComponentFix : public GuiBase {
    static std::array<juce::String, 128> noteLabelText;

    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiToggleButton enable;
    GuiSlider freq;
    GuiSeparator freqToSeparator;
    GuiTextButton freqTo;
    GuiTextButton freqToZero;
    GuiTextButton freqTo05;
    GuiTextButton freqTo1;
    GuiTextButton freqTo2;
    GuiSeparator freqAddSeparator;
    GuiTextButton freqM05;
    GuiTextButton freqM02;
    GuiTextButton freqM01;
    GuiTextButton freqP01;
    GuiTextButton freqP02;
    GuiTextButton freqP05;
    GuiTextButton freqM5;
    GuiTextButton freqM2;
    GuiTextButton freqM1;
    GuiTextButton freqP1;
    GuiTextButton freqP2;
    GuiTextButton freqP5;
    GuiTextButton freqM50;
    GuiTextButton freqM20;
    GuiTextButton freqM10;
    GuiTextButton freqP10;
    GuiTextButton freqP20;
    GuiTextButton freqP50;
    GuiTextButton freqM500;
    GuiTextButton freqM200;
    GuiTextButton freqM100;
    GuiTextButton freqP100;
    GuiTextButton freqP200;
    GuiTextButton freqP500;
    GuiTextButton freqM1000;
    GuiTextButton freqP1000;
    GuiSeparator freqNoteSeparator;
    GuiSlider freqNote;
    GuiLabel note;
    GuiTextButton applyNote;
    GuiTextButton applyToC3;
public:
    GuiComponentFix(const GuiContext& context) :
        GuiBase(context),
        cat(context),
		enable(context),
		freq(context),
        freqToSeparator(context),
		freqTo(context),
        freqToZero(context),
        freqTo05(context),
        freqTo1(context),
        freqTo2(context),
        freqAddSeparator(context),
        freqM05(context),
        freqM02(context),
        freqM01(context),
        freqP01(context),
        freqP02(context),
        freqP05(context),
        freqM5(context),
        freqM2(context),
        freqM1(context),
        freqP1(context),
        freqP2(context),
        freqP5(context),
        freqM50(context),
        freqM20(context),
        freqM10(context),
        freqP10(context),
        freqP20(context),
        freqP50(context),
        freqM500(context),
        freqM200(context),
        freqM100(context),
        freqP100(context),
        freqP200(context),
        freqP500(context),
        freqM1000(context),
        freqP1000(context),
        freqNoteSeparator(context),
        freqNote(context),
        note(context),
        applyNote(context),
        applyToC3(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& toLabel, int toValue);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnables(bool enabled);
};
