#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// 相対パスの基準 (UTILITY)
// ============================================================================
// プリセットやチャンネルパラメータに書く音声・波形ファイルの場所を、どこから
// の相対で書くか (Io::PathRoot) を選ぶ。
//
// 値は環境設定に 1 つだけで、パラメータではない。同じ部品がいくつかの
// チャンネルの UTILITY に並ぶので、どれかで変えたら残りの表示もそろえる。
class GuiComponentPathRoot
{
public:
    GuiComponentPathRoot(const GuiContext& context);
    ~GuiComponentPathRoot();

    void setupComponent(juce::Component& parent, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void setVisible(bool visible);

private:
    GuiContext ctx;
    GuiComboBox selector;

    // 今の値を表示へ入れる
    void refresh();
};
