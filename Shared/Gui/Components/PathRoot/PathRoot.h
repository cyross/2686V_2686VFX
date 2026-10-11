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

    // 渡した 1 行へ、ラベルとコンボボックスを左から並べる (SETTINGS)
    void layoutRow(juce::Rectangle<int> row, int labelWidth, int selectorWidth);

    void setVisible(bool visible);

    // 今の値を表示へ入れる。環境設定を読み直したあとに呼ぶ。
    void refresh();

private:
    GuiContext ctx;
    GuiComboBox selector;
};
