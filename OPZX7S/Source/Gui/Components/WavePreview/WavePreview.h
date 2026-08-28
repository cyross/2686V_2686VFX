#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiContext.h"

// ============================================================================
// 波形プレビュー
// ============================================================================
// 選んだ Shape や読み込んだ波形が「どんな形か」を見せるための小さな表示。
//
// 描画のたびに波形を計算し直すと重くなるので、値が変わったときに描くものを
// 1 つ作り置きし、paint() は持っているものを描くだけにしてある。
//
// 描き方は 2 通りある。
//   折れ線   : 1 点ずつ結ぶ。波形やエンベロープなど、点をそのまま拾って
//              形が分かるもの向け。
//   上下幅   : 区間ごとの最小値と最大値を縦線で結ぶ。オーディオファイルの
//              ように 1 点ずつ拾っても形が分からないもの向け。
//
// 値は -1.0〜1.0 で受け取る。片側だけの波形 (エンベロープなど) は
// bipolar = false を渡すと下端を 0 として描く。
class GuiWavePreview : public juce::Component, public GuiBaseComponent
{
    std::vector<float> m_points;

    // 上下幅で描くときの区間ごとの最小値 / 最大値
    std::vector<float> m_mins;
    std::vector<float> m_maxs;

    // 0.0〜1.0 の位置に引く縦線
    std::vector<float> m_markers;

    bool m_bipolar = true;
    bool m_hasData = false;
    bool m_isEnvelope = false;
public:
    GuiWavePreview(const GuiContext& context) : GuiBaseComponent(context)
    {
        setInterceptsMouseClicks(false, false);
    }

    // 高さの目安。呼び出し側はこれを使って区画を取る。
    static inline constexpr int defaultHeight = 34;

    void setup(juce::Component& parent);

    // 折れ線を差し替える。空を渡すと「データ無し」の表示になる。
    void setPoints(const std::vector<float>& points, bool bipolar = true);

    // 上下幅で描くものへ差し替える。mins と maxs は同じ長さで渡すこと。
    void setEnvelope(const std::vector<float>& mins, const std::vector<float>& maxs);

    // 縦線を引く位置 (0.0〜1.0)。ループ範囲などの目印に使う。
    // 空を渡すと消える。
    void setMarkers(const std::vector<float>& positions);

    void clear();

    void paint(juce::Graphics& g) override;
};
