#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

class GuiComponentLevel : public GuiBase {
    GuiSlider levelSlider;

    GuiComboBox stepSelector;

    GuiTextButton levelPM1;
    GuiTextButton levelTo1;
    GuiTextButton levelP1;
    GuiTextButton levelPM01;
    GuiTextButton levelPM001;
    GuiTextButton levelP001;
    GuiTextButton levelP01;
    GuiTextButton levelTo0125;
    GuiTextButton levelTo0142;
    GuiTextButton levelTo016;
    GuiTextButton levelTo02;
    GuiTextButton levelTo025;
    GuiTextButton levelTo033;
    GuiTextButton levelTo04;
    GuiTextButton levelTo05;
    GuiTextButton levelTo06;
    GuiTextButton levelTo067;
    GuiTextButton levelTo075;
    GuiTextButton levelTo08;
    GuiTextButton levelTo083;

    std::vector<SelectItem> stepItems = {
            { "1: Free", 1 },
            { "2: 16 Steps", 2 },
            { "3: 32 Steps", 3 },
            { "4: 64 Steps", 4 },
            { "5: 128 Steps", 5 },
            { "6: 256 Steps", 6 }
    };

    // 内部でスナップ処理を行うための関数
    void applyStepSnap();
public:
    GuiComponentLevel(const GuiContext& context) :
        GuiBase(context),
        levelSlider(context),
		stepSelector(context),
        levelTo1(context),
        levelPM1(context),
        levelP1(context),
        levelPM01(context),
        levelPM001(context),
        levelP001(context),
        levelP01(context),
        levelTo0125(context),
        levelTo0142(context),
        levelTo016(context),
        levelTo02(context),
        levelTo025(context),
        levelTo033(context),
        levelTo04(context),
        levelTo05(context),
        levelTo06(context),
        levelTo067(context),
        levelTo075(context),
        levelTo08(context),
        levelTo083(context)
    {
    }

    void setupComponent(juce::Component& parent, int& tabOrder, const juce::String& prefix);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setVisible(bool visible);
    void setEnable(bool enable);
    float getLevel();
    void setLevel(float level);
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
