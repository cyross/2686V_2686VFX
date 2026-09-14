#include "./WavePreviewGrid.h"

#include <algorithm>

void GuiWavePreviewGrid::setup(juce::Component& parent, juce::Colour lineColour, int slotCount, int columnCount)
{
    parent.addAndMakeVisible(*this);

    m_lineColour = lineColour;
    columns = juce::jmax(1, columnCount);
    m_slots.assign((size_t)juce::jmax(0, slotCount), {});
    m_mins.assign((size_t)juce::jmax(0, slotCount), {});
    m_maxs.assign((size_t)juce::jmax(0, slotCount), {});
}

void GuiWavePreviewGrid::setPoints(int slot, const std::vector<float>& points)
{
    if (slot < 0 || slot >= (int)m_slots.size()) return;

    m_slots[(size_t)slot] = points;
    m_mins[(size_t)slot].clear();
    m_maxs[(size_t)slot].clear();

    repaint();
}

void GuiWavePreviewGrid::setEnvelope(int slot, const std::vector<float>& mins, const std::vector<float>& maxs)
{
    if (slot < 0 || slot >= (int)m_slots.size()) return;

    m_slots[(size_t)slot].clear();

    // 対になっていないものは描けないので、そのときは空にする
    const bool paired = !mins.empty() && mins.size() == maxs.size();

    m_mins[(size_t)slot] = paired ? mins : std::vector<float>{};
    m_maxs[(size_t)slot] = paired ? maxs : std::vector<float>{};

    repaint();
}

void GuiWavePreviewGrid::setSelected(int slot)
{
    if (m_selected == slot) return;

    m_selected = slot;

    repaint();
}

void GuiWavePreviewGrid::setActive(int slot)
{
    if (m_active == slot) return;

    m_active = slot;

    repaint();
}

void GuiWavePreviewGrid::paint(juce::Graphics& g)
{
    const int count = (int)m_slots.size();

    if (count <= 0) return;

    auto whole = getLocalBounds().toFloat();

    if (whole.isEmpty()) return;

    const int rows = getRowCount();

    const float cellW = whole.getWidth() / (float)columns;
    const float cellH = whole.getHeight() / (float)rows;

    for (int i = 0; i < count; ++i)
    {
        const int col = i % columns;
        const int row = i / columns;

        auto cell = juce::Rectangle<float>(whole.getX() + cellW * (float)col,
                                           whole.getY() + cellH * (float)row,
                                           cellW, cellH).reduced(1.0f);

        // ---------------- 下地 ----------------
        auto bounds = GuiShadow::reserve(cell);

        GuiShadow::drawRounded(g, bounds, guiCornerRadius);

        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.fillRoundedRectangle(bounds, guiCornerRadius);

        // 鳴っているスロットは下地に色を敷く。枠 (読み込み・消去の対象) とは別。
        if (i == m_active) {
            g.setColour(GuiColor::WavePreview::ActiveSlotBg.get());
            g.fillRoundedRectangle(bounds, guiCornerRadius);
        }

        auto area = bounds.reduced(2.0f);

        if (area.getWidth() <= 1.0f || area.getHeight() <= 1.0f) continue;

        // ---------------- 基準線 ----------------
        const float zeroY = area.getCentreY();
        const float scale = area.getHeight() * 0.5f;

        g.setColour(GuiColor::WaveformContainer::Track.get().withAlpha(0.35f));
        g.drawHorizontalLine((int)zeroY, area.getX(), area.getRight());

        // ---------------- 上下幅 ----------------
        const auto& mins = m_mins[(size_t)i];
        const auto& maxs = m_maxs[(size_t)i];

        if (!mins.empty())
        {
            g.setColour(m_lineColour);

            // 区間の数は枠の幅よりずっと多い。そのまま 1 本ずつ引くと
            // 同じ画素へ何度も描くだけなので、画素の数へまとめる。
            const int n = (int)mins.size();
            const int px = juce::jmax(1, (int)area.getWidth());

            for (int x = 0; x < px; ++x) {
                const int from = (int)((int64_t)n * x / px);
                int to = (int)((int64_t)n * (x + 1) / px);

                if (to <= from) to = from + 1;
                if (to > n) to = n;

                float lo = mins[(size_t)from];
                float hi = maxs[(size_t)from];

                for (int k = from + 1; k < to; ++k) {
                    lo = std::min(lo, mins[(size_t)k]);
                    hi = std::max(hi, maxs[(size_t)k]);
                }

                const float px1 = area.getX() + (float)x + 0.5f;

                float top = zeroY - std::clamp(hi, -1.0f, 1.0f) * scale;
                float bottom = zeroY - std::clamp(lo, -1.0f, 1.0f) * scale;

                // 無音の区間でも線が消えないよう、最低 1 ピクセルは引く
                if (bottom - top < 1.0f) bottom = top + 1.0f;

                g.drawLine(px1, top, px1, bottom, 1.0f);
            }
        }

        // ---------------- 折れ線 ----------------
        const auto& points = m_slots[(size_t)i];

        if (!points.empty())
        {
            g.setColour(m_lineColour);

            juce::Path path;

            const int n = (int)points.size();

            for (int k = 0; k < n; ++k) {
                float x = area.getX() + (n <= 1 ? 0.0f : area.getWidth() * (float)k / (float)(n - 1));
                float y = zeroY - std::clamp(points[(size_t)k], -1.0f, 1.0f) * scale;

                if (k == 0) path.startNewSubPath(x, y);
                else        path.lineTo(x, y);
            }

            g.strokePath(path, juce::PathStrokeType(1.2f));
        }

        // ---------------- 選んでいるもの ----------------
        if (i == m_selected)
        {
            g.setColour(GuiColor::StepValues::Selected.get());
            g.drawRoundedRectangle(bounds.reduced(0.5f), guiCornerRadius, 1.0f);
        }

        // 何番のスロットかを左上に小さく出す。鳴っているものは色を変える。
        g.setColour(i == m_active ? GuiColor::WavePreview::ActiveSlot.get()
                                  : GuiColor::StepValues::Text.get());
        g.setFont(juce::Font(juce::FontOptions(9.0f)));
        g.drawText(juce::String(i), area.removeFromTop(10.0f).removeFromLeft(14.0f),
                   juce::Justification::centredLeft);
    }
}
