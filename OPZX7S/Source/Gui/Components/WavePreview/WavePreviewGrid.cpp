#include "./WavePreviewGrid.h"

void GuiWavePreviewGrid::setup(juce::Component& parent, juce::Colour lineColour, int slotCount)
{
    parent.addAndMakeVisible(*this);

    m_lineColour = lineColour;
    m_slots.assign((size_t)juce::jmax(0, slotCount), {});
}

void GuiWavePreviewGrid::setPoints(int slot, const std::vector<float>& points)
{
    if (slot < 0 || slot >= (int)m_slots.size()) return;

    m_slots[(size_t)slot] = points;

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
