#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

#include "../NudgeButtons/NudgeButtons.h"
#include "./LevelNudge.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../Separator/NormalSeparator.h"

class GuiComponentLevel : public GuiBase {
    GuiComponentNudgeSliderFloat levelSlider;

    // 押してから鳴り始めるまでの間 (秒)。
    // この部品はどのチャンネルも使うので、ここへ置けば全チャンネルに入る。
    NormalSeparator delaySeparator;
    GuiComponentNudgeSliderFloat delaySlider;
    GuiComponentNudgeButtons delayNudge;

    GuiComboBox stepSelector;

    // N ボタンで出す、値を動かすボタンの並び。RHYTHM のパッドと共用。
    GuiComponentLevelNudge levelNudge;

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
        delaySeparator(context),
        delaySlider(context),
        delayNudge(context),
		stepSelector(context),
        levelNudge(context)
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
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
