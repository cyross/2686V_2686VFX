#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

#include "../NudgeSlider/NudgeSliderFloat.h"

class GuiComponentLevel : public GuiBase {
    GuiComponentNudgeSliderFloat levelSlider;

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
        { juce::String("") + " 1: 自由設定", 1 },
        { juce::String("") + " 2: 1 刻み", 2 },
        { juce::String("") + " 3: 1 / 2 刻み", 3 },
        { juce::String("") + " 4: 1 / 4 刻み", 4 },
        { juce::String("") + " 5: 1 / 8 刻み", 5 },
        { juce::String("") + " 6: 1 / 16 刻み", 6 },
        { juce::String("") + " 7: 1 / 32 刻み", 7 },
        { juce::String("") + " 8: 1 / 64 刻み", 8 },
        { juce::String("") + " 9: 1 / 128 刻み", 9 },
        { juce::String("") + "10: 1 / 256 刻み", 10 },
        { juce::String("") + "11: 1 / 3 刻み", 11 },
        { juce::String("") + "12: 1 / 6 刻み", 12 },
        { juce::String("") + "13: 1 / 7 刻み", 13 },
        { juce::String("") + "14: 1 / 10 刻み", 14 },
        { juce::String("") + "15: 1 / 100 刻み", 15 }
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

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& prefix);
    void writeParams(Io::ParamWriter& writer, const juce::String& prefix);
    juce::String getExportedParams();
};
