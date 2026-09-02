#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <functional>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "../../../Gui/Components/SsgSwButtons/SsgSwButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentNudgeSliderFloat : public GuiBase {
    bool visibleNudgeButtons = false;

    GuiSlider mainSlider;
    GuiTextButton nudgeVisibleButton;
public:
    // ナッジ表示切り替えボタンの幅と、スライダーとの間隔
    static inline constexpr int nudgeButtonWidth = 20;
    static inline constexpr int nudgeButtonGap = 1;

    GuiComponentNudgeSliderFloat(const GuiContext& context) :
        GuiBase(context),
        mainSlider(context),
        nudgeVisibleButton(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& id, const juce::String& title, int& tabOrder, std::optional<std::function<void ()>> onValueChange, std::optional<juce::Font> labelFont = std::nullopt, bool isReset = true);

    // rowHeight は layoutMain / layoutRow の rowHeight と同じ意味。既定値も揃えてあるので、
    // 差し替え元が rowHeight を指定していた場合だけ引き継げばよい。
    void layoutComponent(juce::Rectangle<int>& rect, int rowHeight = CoreGuiValue::MainGroup::Row::height);
    void layoutComponentRow(juce::Rectangle<int>& rect, int rowHeight = CoreGuiValue::ParamGroup::Row::height);

    void setVisibles(bool visible);
    void setEnabled(bool enabled);
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
    float getValue() {
        return mainSlider.getValue();
    };
    void setValue(float value) {
        mainSlider.setValue(value, juce::sendNotification);
    }
    void updateVisibleNudge(bool visibleNudge) {
        this->visibleNudgeButtons = visibleNudge;
    }
    bool isVisibleNudge() {
        return this->visibleNudgeButtons;
    }
    float getMinimum() { return mainSlider.getMinimum(); }
    float getMaximum() { return mainSlider.getMaximum(); }

    // GuiSlider を直接受け取る既存コード向けの入口。
    // (GuiComponentPitchButtons / GuiComponentSsgSwButtons の setupComponent、
    //  グラフ再描画用の onValueChange、UNISON のパラメータ差し替え rebind など)
    GuiSlider& getSlider() { return mainSlider; }

    // GuiSlider と同名で呼べるようにしておくと、差し替え時の変更が最小で済む
    void setVisibleWithLabel(bool visible) { setVisibles(visible); }
    void setEnabledWithLabel(bool enabled) { setEnabled(enabled); }
    void setValue(float value, juce::NotificationType notification) {
        mainSlider.setValue(value, notification);
    }
};
