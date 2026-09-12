#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"

// 回数を刻むためのボタン群。2 段で 1 組。
//
//   上: -10  -1  0  +1  +10
//   下: -1000  -100  +100  +1000
//
// 回数は 0〜3000 と幅が広く、1 ずつでは遠く、100 ずつでは粗い。
// 桁の違う刻みを 2 段に分けて、どちらからでも寄せられるようにしてある。
// 0 は上の段の真ん中に置く (0 は「無限に回す」の意味を持つため、
// いちばん押しやすいところへ)。
class GuiComponentCountButtons : public GuiBase {
    GuiTextButton minus10;
    GuiTextButton minus1;
    GuiTextButton pm0;
    GuiTextButton plus1;
    GuiTextButton plus10;

    GuiTextButton minus1000;
    GuiTextButton minus100;
    GuiTextButton plus100;
    GuiTextButton plus1000;

public:
    // 2 段ぶんの高さ。場所を取る側はこれを見て区画を取る。
    static int getHeight(int rowHeight) { return rowHeight * 2; }

    GuiComponentCountButtons(const GuiContext& context) :
        GuiBase(context),
        minus10(context),
        minus1(context),
        pm0(context),
        plus1(context),
        plus10(context),
        minus1000(context),
        minus100(context),
        plus100(context),
        plus1000(context)
    {
    }

    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font = nullopt);
    void layoutComponent(juce::Rectangle<int>& rect, int height = 15);
    void layoutComponentRow(juce::Rectangle<int>& rect, int height = 14);
    void setVisibles(bool visible);
    void setEnables(bool enabled);
};
