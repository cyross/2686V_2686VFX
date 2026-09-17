#pragma once

#include "../../../Core/Gui/GuiI18n.h"
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
        { I18n::pick(u8" 1: 自由設定", u8" 1: Free"), 1 },
        { I18n::pick(u8" 2: 1 刻み", u8" 2: by 1"), 2 },
        { I18n::pick(u8" 3: 1 / 2 刻み", u8" 3: by 1 / 2"), 3 },
        { I18n::pick(u8" 4: 1 / 4 刻み", u8" 4: by 1 / 4"), 4 },
        { I18n::pick(u8" 5: 1 / 8 刻み", u8" 5: by 1 / 8"), 5 },
        { I18n::pick(u8" 6: 1 / 16 刻み", u8" 6: by 1 / 16"), 6 },
        { I18n::pick(u8" 7: 1 / 32 刻み", u8" 7: by 1 / 32"), 7 },
        { I18n::pick(u8" 8: 1 / 64 刻み", u8" 8: by 1 / 64"), 8 },
        { I18n::pick(u8" 9: 1 / 128 刻み", u8" 9: by 1 / 128"), 9 },
        { I18n::pick(u8"10: 1 / 256 刻み", u8"10: by 1 / 256"), 10 },
        { I18n::pick(u8"11: 1 / 3 刻み", u8"11: by 1 / 3"), 11 },
        { I18n::pick(u8"12: 1 / 6 刻み", u8"12: by 1 / 6"), 12 },
        { I18n::pick(u8"13: 1 / 7 刻み", u8"13: by 1 / 7"), 13 },
        { I18n::pick(u8"14: 1 / 10 刻み", u8"14: by 1 / 10"), 14 },
        { I18n::pick(u8"15: 1 / 100 刻み", u8"15: by 1 / 100"), 15 }
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
