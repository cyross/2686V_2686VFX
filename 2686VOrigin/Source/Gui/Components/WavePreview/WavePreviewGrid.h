#pragma once

#include <JuceHeader.h>

#include <vector>

#include "./WavePreview.h"

// ============================================================================
// 波形プレビューを並べて 1 つの区画へ描く
// ============================================================================
// スロットごとに GuiWavePreview を置くと、スロットの数だけ部品が増える。
// 中身は同じ描き方なので、1 つの部品へまとめて格子に並べる。
//
// 値は GuiWavePreview と同じく -1.0〜1.0 で受け取る。
class GuiWavePreviewGrid : public juce::Component, public GuiBaseComponent
{
    std::vector<std::vector<float>> m_slots;

    juce::Colour m_lineColour = GuiColor::WavePreview::WaveMemory;

    int m_selected = -1;
public:
    GuiWavePreviewGrid(const GuiContext& context) : GuiBaseComponent(context)
    {
        setInterceptsMouseClicks(false, false);
    }

    // 1 行に並べる数。値の帯と同じ数にそろえてある。
    static inline constexpr int columns = 3;

    // 1 つぶんの高さ。GuiWavePreview と同じ。
    static inline constexpr int cellHeight = GuiWavePreview::defaultHeight;

    void setup(juce::Component& parent, juce::Colour lineColour, int slotCount);

    // 1 つぶんの波形を差し替える。空を渡すと「データ無し」になる。
    void setPoints(int slot, const std::vector<float>& points);

    // いま読み込み・クリアの対象になっているスロット。枠を強調する。
    void setSelected(int slot);

    int getRowCount() const
    {
        const int n = (int)m_slots.size();

        return (n <= 0) ? 0 : ((n + columns - 1) / columns);
    }

    int getNaturalHeight() const { return getRowCount() * cellHeight; }

    void paint(juce::Graphics& g) override;
};
