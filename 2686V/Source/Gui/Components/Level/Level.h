#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

class GuiComponentLevel : public GuiBase {
    GuiSlider levelSlider;
    GuiTextButton levelPM1;
    GuiTextButton levelTo1;
    GuiTextButton levelP1;
    GuiTextButton levelPM01;
    GuiTextButton levelPM001;
    GuiTextButton levelP001;
    GuiTextButton levelP01;
public:
    GuiComponentLevel(const GuiContext& context) :
        GuiBase(context),
        levelSlider(context),
        levelPM1(context),
        levelTo1(context),
        levelP1(context),
        levelPM01(context),
        levelPM001(context),
        levelP001(context),
        levelP01(context)
    {
    }

    void setupComponent(juce::Component& parent, int& tabOrder, const juce::String& prefix);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setVisible(bool visible);
    void setEnable(bool enable);
    float getLevel();
    void setLevel(float level);
};
