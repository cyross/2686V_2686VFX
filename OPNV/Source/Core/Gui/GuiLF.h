#pragma once

#include <JuceHeader.h>

class CustomTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // タブの文字の大きさ。幅を測るときと描くときで必ず同じ値を使うこと。
    static inline constexpr float tabFontHeight = 16.0f;

    // 文字の左右に確保する余白
    static inline constexpr int tabPaddingX = 10;

    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override;
    int getTabButtonBestWidth(juce::TabBarButton& button, int tabDepth) override;
    juce::Colour getTabHeaderColor(int tabIndex);
};
