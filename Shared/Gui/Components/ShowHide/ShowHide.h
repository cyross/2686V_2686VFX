#pragma once

#include <JuceHeader.h>

#include <functional>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// 区分の中のまとまりを出し入れするボタン (4.5.0)
// ============================================================================
// ボタンはまとまりの区切り線の下・中身の上に置き、出しているあいだは
// 「HIDE xxx」、隠しているあいだは「SHOW xxx」と出す。隠すのは、まとまりと、
// その上の区切り線。初めは隠しておく。
//
// 並べる側は次のように使う:
//
//     const bool show = visible && holdToggle.isShown();
//     holdSeparator.setVisible(show);   // まとまりの部品
//     holdToggle.setVisible(visible);
//     if (show) holdSeparator.layoutComponent(rect);
//     holdToggle.layoutComponent(rect);
//     if (show) { ...まとまりを並べる... }
//
// 出し入れの状態は画面の中だけで持ち、保存しない (区分の開閉と同じ)。
class GuiShowHideButton : public GuiTextButton
{
public:
    explicit GuiShowHideButton(const GuiContext& context) : GuiTextButton(context) {}

    // name は「HOLD」「PM KEEP」など。ボタンには「SHOW name」「HIDE name」と出す
    void setupComponent(juce::Component& parent, const juce::String& name, int& tabOrder);

    bool isShown() const { return shown; }

    // 出すか隠すかを変える。変わったら並べ直す
    void setShown(bool shouldShow);

    // 上を少し空けて、行より少し低いボタンを置く
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);

    juce::String getPartsName() const { return partsName; }

private:
    void applyLook();

    juce::String partsName;
    bool shown = false;
};
