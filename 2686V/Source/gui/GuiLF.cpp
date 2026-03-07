#include "GuiLF.h"

#include "../gui/GuiText.h"

// アイコンのパスを生成するヘルパー関数
juce::Path CustomTabLookAndFeel::getIconPath(const juce::String& name, juce::Rectangle<float> area)
{
    juce::Path p;
    auto center = area.getCentre();
    float s = std::min(area.getWidth(), area.getHeight()) * 0.5f; // アイコンサイズ

    if (name == GuiText::Tab::fx) // エフェクトを示すアイコン
    {
        // [FX] Icon
        juce::Path fPath;
        fPath.addRectangle(center.x - s * 0.8f, center.y - s * 0.6f, s * 0.2f, s * 1.2f); // 縦棒
        fPath.addRectangle(center.x - s * 0.6f, center.y - s * 0.6f, s * 0.5f, s * 0.2f); // 横棒上
        fPath.addRectangle(center.x - s * 0.6f, center.y - s * 0.1f, s * 0.3f, s * 0.2f); // 横棒中
        p.addPath(fPath);

        // X
        juce::Path xPath;
        float xw = s * 0.2f;
        float xh = s * 1.2f;
        float xPivot = center.x + s * 0.4f; // Xの交差点（中心）

        // Xのクロスを、同じ中心位置にある長方形を左右に傾けて表現する
        juce::Path bar1;
        bar1.addRectangle(xPivot - xw * 0.5f, center.y - xh * 0.5f, xw, xh);
        bar1.applyTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 0.15f, xPivot, center.y));

        juce::Path bar2;
        bar2.addRectangle(xPivot - xw * 0.5f, center.y - xh * 0.5f, xw, xh);
        bar2.applyTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::pi * 0.15f, xPivot, center.y));

        p.addPath(bar1);
        p.addPath(bar2);
    }
    else if (name == GuiText::Tab::preset) // プリセットブラウザタブはフォルダーアイコン
    {
        // --- Folder / List Icon ---
        // フォルダーの形
        float w = s * 1.2f;
        float h = s * 0.9f;
        juce::Rectangle<float> folderRect(center.x - w / 2, center.y - h / 2 + s * 0.1f, w, h);
        p.addRoundedRectangle(folderRect, 2.0f);
        // タブ部分
        p.addRectangle(center.x - w / 2, center.y - h / 2 - s * 0.2f, w * 0.4f, s * 0.3f);

        // 中身の線 (リストっぽく)
        p.startNewSubPath(center.x - w * 0.3f, center.y);
        p.lineTo(center.x + w * 0.3f, center.y);
        p.startNewSubPath(center.x - w * 0.3f, center.y + h * 0.3f);
        p.lineTo(center.x + w * 0.3f, center.y + h * 0.3f);
    }
    else if (name == GuiText::Tab::settings) // 設定タブは歯車アイコン
    {
        // --- Gear Icon ---
        // 歯車
        float rOut = s * 0.6f;
        float rIn = s * 0.45f;
        float rHole = s * 0.2f;

        p.addCentredArc(center.x, center.y, rIn, rIn, 0, 0, juce::MathConstants<float>::twoPi, true);

        // 歯を描く (8個)
        for (int i = 0; i < 8; ++i)
        {
            float angle = i * juce::MathConstants<float>::twoPi / 8.0f;
            juce::Path tooth;
            tooth.addRectangle(center.x - s * 0.12f, center.y - rOut, s * 0.24f, rOut - rIn + 2.0f);
            tooth.applyTransform(juce::AffineTransform::rotation(angle, center.x, center.y));
            p.addPath(tooth);
        }
        // 真ん中の穴
        p.addEllipse(center.x - rHole, center.y - rHole, rHole * 2, rHole * 2);
        p.setUsingNonZeroWinding(false); // 穴を抜く設定
    }
    else if (name == GuiText::Tab::about) // Aboutタブは丸囲みiアイコン
    {
        // --- Info Icon (i) ---
        // 丸
        float r = s * 0.6f;
        p.addEllipse(center.x - r, center.y - r, r * 2, r * 2);

        // i の点
        float dotSize = s * 0.15f;
        juce::Path dot;
        dot.addEllipse(center.x - dotSize / 2, center.y - r * 0.5f, dotSize, dotSize);
        p.addPath(dot);

        // i の棒
        juce::Path bar;
        bar.addRectangle(center.x - dotSize / 2, center.y - r * 0.1f, dotSize, r * 0.8f);
        p.addPath(bar);

        p.setUsingNonZeroWinding(false); // 穴を抜く設定
    }
    return p;
}

void CustomTabLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown)
{
    auto area = button.getActiveArea().toFloat();
    juce::String name = button.getButtonText();

    // 1. タブ背景の描画
    // 選択中タブの背景色
    auto selectedBg = juce::Colour::fromFloatRGBA(0.4f, 0.4f, 0.4f, 1.0f);
    // 非選択タブの背景色
    auto unselectedBg = juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f);

    if (button.isFrontTab()) {
        g.setColour(selectedBg);
    }
    else {
        g.setColour(unselectedBg);
        // マウスオーバーで背景も少し明るく
        if (isMouseOver) g.setColour(unselectedBg.brighter(0.1f));
    }
    g.fillRect(area);

    // 2. コンテンツ（テキストまたはアイコン）の色決定
    juce::Colour contentColour;
    if (button.isFrontTab()) {
        // 選択時は完全な白（強調）
        contentColour = juce::Colours::white;
    }
    else {
        // 非選択時はグレー
        contentColour = juce::Colours::grey;
        // マウスオーバー時は少し明るくして反応させる
        if (isMouseOver) contentColour = contentColour.brighter(0.3f);
    }

    // 3. 描画
    g.setColour(contentColour); // 色を確定

    if (name == GuiText::Tab::fx || name == GuiText::Tab::preset || name == GuiText::Tab::settings || name == GuiText::Tab::about)
    {
        // アイコン描画
        juce::Path icon = getIconPath(name, area);
        g.fillPath(icon);
    }
    else
    {
        // テキスト描画
        g.setFont(juce::Font(16.0f).withStyle(button.isFrontTab() ? juce::Font::bold : juce::Font::plain));
        g.drawText(name, area, juce::Justification::centred, true);
    }
}
