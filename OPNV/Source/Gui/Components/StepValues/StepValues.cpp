#include "./StepValues.h"

void GuiStepValues::paint(juce::Graphics& g)
{
    const int count = (int)values.size();

    if (count <= 0) return;

    auto area = getLocalBounds().toFloat();

    g.setColour(GuiColor::StepValues::Bg.get());
    g.fillRect(area);

    const int rows = getRowCount();

    const float cellW = area.getWidth() / (float)columns;
    const float cellH = area.getHeight() / (float)rows;

    // 行の高さから決める。上限は、行を詰めたときに潰れない大きさ。
    juce::Font font(juce::FontOptions(juce::jmin(13.0f, cellH * 0.8f)));

    for (int i = 0; i < count; ++i)
    {
        const int col = i % columns;
        const int row = i / columns;

        auto cell = juce::Rectangle<float>(area.getX() + cellW * (float)col,
                                           area.getY() + cellH * (float)row,
                                           cellW, cellH);

        const bool isSelected = (i == selected);
        const bool isRelease = lastIsRelease && (i == count - 1);
        const bool isActive = (activeCount < 0) || (i < activeCount) || isRelease;

        if (isSelected)
        {
            g.setColour(GuiColor::StepValues::SelectedBg.get());
            g.fillRect(cell.reduced(0.5f));
        }

        g.setColour(isSelected ? GuiColor::StepValues::Selected.get()
                               : (isActive ? GuiColor::StepValues::Text.get()
                                           : GuiColor::StepValues::DimText.get()));

        g.setFont(font);

        // 見出しは左、値は右。値のほうが桁を食うので広く取る。
        auto inner = cell.reduced(2.0f, 0.0f);
        auto labelArea = inner.removeFromLeft(inner.getWidth() * 0.4f);

        if (i < (int)labels.size()) {
            g.drawFittedText(labels[(size_t)i], labelArea.toNearestInt(), juce::Justification::centredLeft, 1, 0.7f);
        }

        g.drawFittedText(juce::String(values[(size_t)i], decimals), inner.toNearestInt(),
                         juce::Justification::centredRight, 1, 0.7f);
    }

    g.setColour(GuiColor::StepValues::Frame.get());
    g.drawRect(area, 1.0f);
}
