#include "./GuiLF.h"

#include "./GuiColor.h"

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
    g.setFont(juce::Font(juce::FontOptions(tabFontHeight)).withStyle(button.isFrontTab() ? juce::Font::bold : juce::Font::plain));
    g.drawText(name, area, juce::Justification::centred, true);
}

// タブの幅を決める。
//
// 既定の実装はタブの高さから割り出した文字サイズで測るため、ここで
// 実際に描いている大きさとずれる。しかも選択中のタブだけ太字で描くので、
// 細字で測ると選択した瞬間に文字が入りきらず "..." で省略されてしまう。
// いちばん広くなる状態 (太字) で測って、そこへ余白を足す。
int CustomTabLookAndFeel::getTabButtonBestWidth(juce::TabBarButton& button, int tabDepth)
{
    auto font = juce::Font(juce::FontOptions(tabFontHeight)).withStyle(juce::Font::bold);

    int textWidth = juce::GlyphArrangement::getStringWidthInt(font, button.getButtonText().trim());

    // 描画は getActiveArea() の中で行われ、左右から画像用の余白が引かれる。
    // そのぶんも足しておかないと、足した余白が食われてしまう。
    int width = textWidth + getTabButtonSpaceAroundImage() * 2 + tabPaddingX * 2;

    // 短い名前のタブが細くなりすぎないよう、下限だけ既定と同じにしておく
    return juce::jmax(tabDepth * 2, width);
}

// =======================================================
// 各タブのインデックスに対応したヘッダカラーを定義
// =======================================================
juce::Colour CustomTabLookAndFeel::getTabHeaderColor(int tabIndex)
{
    // 色そのものは GuiColor::Tab にある。タブの並びはプラグインごとに
    // 違うので、どの番号がどの系統かだけをここで決める。
    switch (tabIndex)
    {
    case 0: return GuiColor::Tab::Fm;      // OPN
    case 1: return GuiColor::Tab::Ssg;     // SSG
    case 2: return GuiColor::Tab::Utility; // PRESET
    case 3: return GuiColor::Tab::Utility; // SETTINGS
    case 4: return GuiColor::Tab::Utility; // COLORS
    case 5: return GuiColor::Tab::Utility; // ABOUT
    default: return GuiColor::Tab::Other;  // OTHER
    }
}
