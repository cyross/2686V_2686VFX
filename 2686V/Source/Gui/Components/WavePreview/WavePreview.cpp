#include "./WavePreview.h"

#include "../../../Core/Gui/GuiColor.h"

void GuiWavePreview::setup(juce::Component& parent, juce::Colour lineColour)
{
    parent.addAndMakeVisible(this);

    m_lineColour = lineColour;
}

void GuiWavePreview::setLineColour(juce::Colour lineColour)
{
    if (m_lineColour == lineColour) return;

    m_lineColour = lineColour;

    repaint();
}

void GuiWavePreview::setPoints(const std::vector<float>& points, bool bipolar)
{
    m_points = points;
    m_mins.clear();
    m_maxs.clear();

    m_bipolar = bipolar;
    m_isEnvelope = false;
    m_hasData = !points.empty();

    repaint();
}

void GuiWavePreview::setEnvelope(const std::vector<float>& mins, const std::vector<float>& maxs)
{
    m_points.clear();
    m_mins = mins;
    m_maxs = maxs;

    m_bipolar = true;
    m_isEnvelope = true;
    m_hasData = !mins.empty() && mins.size() == maxs.size();

    repaint();
}

void GuiWavePreview::setMarkers(const std::vector<float>& positions)
{
    m_markers = positions;

    repaint();
}

void GuiWavePreview::clear()
{
    m_points.clear();
    m_mins.clear();
    m_maxs.clear();
    m_markers.clear();

    m_hasData = false;
    m_isEnvelope = false;

    repaint();
}

void GuiWavePreview::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    if (bounds.isEmpty()) return;

    // ---------------- 下地 ----------------
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillRoundedRectangle(bounds, 2.0f);

    auto area = bounds.reduced(2.0f);

    if (area.getWidth() <= 1.0f || area.getHeight() <= 1.0f) return;

    // ---------------- 基準線 ----------------
    // 両振りなら中央、片側なら下端が 0 になる。
    float zeroY = m_bipolar ? area.getCentreY() : area.getBottom();
    float scale = m_bipolar ? (area.getHeight() * 0.5f) : area.getHeight();

    g.setColour(GuiColor::WaveformContainer::Track.withAlpha(0.35f));
    g.drawHorizontalLine((int)zeroY, area.getX(), area.getRight());

    if (m_hasData) {
        g.setColour(m_lineColour);

        if (m_isEnvelope) {
            // ---------------- 上下幅 ----------------
            const int n = (int)m_mins.size();

            for (int i = 0; i < n; ++i) {
                float x = area.getX() + area.getWidth() * ((float)i + 0.5f) / (float)n;
                float top = zeroY - std::clamp(m_maxs[(size_t)i], -1.0f, 1.0f) * scale;
                float bottom = zeroY - std::clamp(m_mins[(size_t)i], -1.0f, 1.0f) * scale;

                // 無音の区間でも線が消えないよう、最低 1 ピクセルは引く
                if (bottom - top < 1.0f) bottom = top + 1.0f;

                g.drawLine(x, top, x, bottom, 1.0f);
            }
        }
        else {
            // ---------------- 折れ線 ----------------
            juce::Path path;

            const int n = (int)m_points.size();

            for (int i = 0; i < n; ++i) {
                float x = area.getX() + (n <= 1 ? 0.0f : area.getWidth() * (float)i / (float)(n - 1));
                float y = zeroY - std::clamp(m_points[(size_t)i], -1.0f, 1.0f) * scale;

                if (i == 0) path.startNewSubPath(x, y);
                else        path.lineTo(x, y);
            }

            g.strokePath(path, juce::PathStrokeType(1.2f));
        }
    }

    // ---------------- 目印 ----------------
    // 波形の上へ重ねるので、最後に引く。
    g.setColour(GuiColor::Category::MarkerOpen);

    for (float pos : m_markers) {
        float x = area.getX() + area.getWidth() * std::clamp(pos, 0.0f, 1.0f);

        g.drawLine(x, area.getY(), x, area.getBottom(), 1.0f);
    }
}
