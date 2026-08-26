#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"

// -1.0 / -0.1 / -0.01 / 0.0 / +0.01 / +0.1 / +1.0 の 7 個で 1 行を構成する
// 汎用のナッジボタン群。GuiComponentNudgeSliderFloat の N ボタンと組にして使う。
class GuiComponentNudgeButtons : public GuiBase {
    GuiTextButton minus1;
    GuiTextButton minus01;
    GuiTextButton minus001;
    GuiTextButton pm0;
    GuiTextButton plus001;
    GuiTextButton plus01;
    GuiTextButton plus1;

public:
    // 7 個を 1 行に詰めるため、他のボタン群 (6 個 / 4 個) より小さめの文字にする。
    // 1 個あたりの幅は (行幅 - 内側余白 * 6) / 7 で、6 個並びより約 15% 狭い。
    // "-0.01" / "+0.01" の 5 文字が収まる大きさが目安。
    static inline constexpr float defaultFontHeight = 10.0f;

    GuiComponentNudgeButtons(const GuiContext& context) :
        GuiBase(context),
        minus1(context),
        minus01(context),
        minus001(context),
        pm0(context),
        plus001(context),
        plus01(context),
        plus1(context)
    {
    }

    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font = nullopt);
    void layoutComponent(juce::Rectangle<int>& rect, int height = 15);
    void layoutComponentRow(juce::Rectangle<int>& rect, int height = 14);
    void setVisibles(bool visible);
    void setEnables(bool enabled);
};
