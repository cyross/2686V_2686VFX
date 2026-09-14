#include "./TargetCell.h"

#include "../../../Core/Gui/GuiColor.h"

namespace
{
    // 枠の内側へ残す余白。枠線が見切れないだけの幅を取る。
    constexpr int inset = 4;

    constexpr int titleHeight = 14;
    constexpr int gap = 2;

    // グラフの丈。オペレータやパッドを 4 つ横に並べても縦に収まる高さ。
    constexpr int graphHeight = 120;
}

int GuiTargetCell::naturalHeight(bool withPreview)
{
    int h = inset * 2 + titleHeight + gap + graphHeight;

    if (withPreview) h += GuiWavePreview::defaultHeight + gap;

    return h;
}

void GuiTargetCell::setup(juce::Component& parent, int index, const juce::String& title, bool withPreview)
{
    m_index = index;
    hasPreview = withPreview;

    parent.addAndMakeVisible(this);

    titleLabel.setup({ .parent = *this, .title = title });

    if (hasPreview) m_preview.setup(*this, GuiColor::WavePreview::AudioFile);

    addAndMakeVisible(&m_graph);

    // 枠のどこを押しても TARGET が動くようにする。
    // 見出しとグラフが押下を食べてしまうと、隙間しか当たらない。
    titleLabel.setInterceptsMouseClicks(false, false);
    m_graph.setInterceptsMouseClicks(false, false);
}

void GuiTargetCell::layout(juce::Rectangle<int> rect)
{
    setBounds(rect);

    auto area = getLocalBounds().reduced(inset);

    titleLabel.setBounds(area.removeFromTop(titleHeight));

    area.removeFromTop(gap);

    if (hasPreview) {
        m_preview.setBounds(area.removeFromTop(GuiWavePreview::defaultHeight));

        area.removeFromTop(gap);
    }

    m_graph.setBounds(area);
}

void GuiTargetCell::setActive(bool active)
{
    if (isActive == active) return;

    isActive = active;

    repaint();
}

void GuiTargetCell::paint(juce::Graphics& g)
{
    // TARGET が指している枠だけ濃く囲う。指していない枠も薄く囲って
    // おかないと、どこまでが 1 枚なのか分からなくなる。
    g.setColour(isActive ? GuiColor::TargetFrame::Active.get() : GuiColor::TargetFrame::Border.get());
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 3.0f, isActive ? 2.0f : 1.0f);
}

void GuiTargetCell::mouseDown(const juce::MouseEvent&)
{
    // 枠を押したらそこへ TARGET を移す。下の設定もまとめて切り替わる。
    if (onSelect) onSelect(m_index);
}
