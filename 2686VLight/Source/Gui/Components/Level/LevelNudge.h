#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

// ==========================================================
// LEVEL の N ボタンで出す、値を動かすボタンの並び
// ==========================================================
// ±1.0 / ±0.1 / ±0.01 で動かすものと、1.0・0.5・1/3 のような
// よく使う値へ直に置くものを 5 行に並べる。
//
// チャンネルの LEVEL と RHYTHM のパッドの LEVEL で同じものを使う。
// どちらも範囲は同じなので、並びも同じにしてある。
class GuiComponentLevelNudge : public GuiBase {
public:
    // ボタンの数。5 行ぶん (3 / 3 / 4 / 6 / 4)。
    static inline constexpr int count = 20;

private:
    std::array<std::unique_ptr<GuiTextButton>, count> buttons;

public:
    GuiComponentLevelNudge(const GuiContext& context);

    // 押したときに動かすつまみを渡す。TARGET で繋ぎ替えるつまみでも、
    // つまみそのものは同じなのでそのまま効く。
    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder);

    // チャンネルの設定 (本体の列) に置くとき
    void layoutComponent(juce::Rectangle<int>& rect);

    // 区分の中に置くとき
    void layoutComponentRow(juce::Rectangle<int>& rect);

    void setVisibles(bool visible);
    void setEnables(bool enabled);
};
