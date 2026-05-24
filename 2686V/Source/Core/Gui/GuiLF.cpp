#include "GuiLF.h"

#include "./GuiText.h"

void CustomTabLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown)
{
    auto area = button.getActiveArea().toFloat();
    auto& bar = button.getTabbedButtonBar();
    int tabIndex = bar.indexOfTabButton(&button); // 現在描画しようとしているタブのインデックス
    bool isFrontTab = button.isFrontTab(); // 選択されているかどうか
    juce::String name = button.getButtonText();
    juce::Colour baseColor = getTabHeaderColor(tabIndex); // 定義したカスタムカラーを取得

    // 状態に合わせて色の明るさや透明度を微調整
    if (isFrontTab) {
        baseColor = baseColor.brighter(0.2f).withAlpha(1.0f); // 選択中は明るく・不透明
    }
    else if (isMouseOver) {
        baseColor = baseColor.withAlpha(0.8f);                // ホバー中は少し濃く
    }
    else {
        baseColor = baseColor.withAlpha(0.5f);                // 非選択時は暗く・半透明
    }

    // 1. タブの背景を塗りつぶし
    g.setColour(baseColor);
    g.fillRect(area);

    // 2. 枠線を描画（隣のタブとの境界線）
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRect(area, 1);

    // 2. コンテンツ（テキストまたはアイコン）の色決定
    juce::Colour contentColour = isFrontTab ? juce::Colours::white : contentColour = juce::Colours::black;

    if (!isFrontTab && isMouseOver) contentColour = contentColour.brighter(0.3f);

    // 3. 描画
    g.setColour(contentColour); // 色を確定

    // テキスト描画
    g.setFont(juce::Font(16.0f).withStyle(button.isFrontTab() ? juce::Font::bold : juce::Font::plain));
    g.drawText(name, area, juce::Justification::centred, true);
}

// =======================================================
// 各タブのインデックスに対応したヘッダカラーを定義
// =======================================================
juce::Colour CustomTabLookAndFeel::getTabHeaderColor(int tabIndex)
{
    // お好みの色を指定してください（アルファ値で濃さを調整すると綺麗です）
    switch (tabIndex)
    {
    case 0: return juce::Colours::darkgreen;      // OPNA
    case 1: return juce::Colours::darkgreen;      // OPN
    case 2: return juce::Colours::darkgreen;      // OPL
    case 3: return juce::Colours::darkgreen;      // OPL3
    case 4: return juce::Colours::darkgreen;      // OPM
    case 5: return juce::Colours::darkgreen;      // OPZX7
    case 6: return juce::Colours::darkcyan;       // SSG
    case 7: return juce::Colours::darkcyan;       // WT
    case 8: return juce::Colours::darkcyan;       // RHYTHM
    case 9: return juce::Colours::darkcyan;       // ADPCM
    case 10: return juce::Colours::darkcyan;      // BEEP
    case 11: return juce::Colours::darkred;       // ADVANCED
    case 12: return juce::Colours::darkgoldenrod; // PRESET
    case 13: return juce::Colours::darkgoldenrod; // SETTINGS
    case 14: return juce::Colours::darkgoldenrod; // ABOUT
    default: return juce::Colours::darkgrey;      // OTHER
    }
}
