#pragma once

#include <JuceHeader.h>

// ============================================================================
// 壁紙 (4.5.0)
// ============================================================================
// 画像を読み込み、窓の背景として描く。生成過程プレビューの窓が使う。
// 描き方はプラグインの画面の壁紙 (SETTINGS の「壁紙」) と同じ。
//
//   mode 0: Stretch  … 区画いっぱいに引き伸ばす
//   mode 1: Fill     … 画像の縦横比のまま、区画を覆う
//   mode 2: Fit      … 画像の縦横比のまま、区画に収める。余白にはぼかした画像を敷く
//   mode 3: Original … そのままの大きさで真ん中に置く。余白にはぼかした画像を敷く
//
// 描いたあとは手前の部品が読めるよう、薄く暗い膜を掛ける。
class GuiWallpaper
{
public:
    // path の画像を読む。前と同じ path なら読み直さない。
    // 空・読めないときは画像を持たない。変わったら true
    bool load(const juce::String& path);

    bool isValid() const { return m_image.isValid(); }

    // area へ描く。画像を持っていなければ何もせず false
    bool draw(juce::Graphics& g, juce::Rectangle<float> area, int mode) const;

private:
    juce::String m_path;
    bool m_loaded = false;
    juce::Image m_image;
    juce::Image m_blurred;
};
