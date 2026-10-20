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

        // 「見出し: 値」を 1 つにして並べ、隣とは「 / 」で区切る。
        auto inner = cell.reduced(2.0f, 0.0f);

        if (col > 0) {
            auto slash = inner.removeFromLeft(juce::GlyphArrangement::getStringWidth(font, " / "));

            g.setColour(GuiColor::StepValues::DimText.get());
            g.drawText("/", slash, juce::Justification::centred);
            g.setColour(isSelected ? GuiColor::StepValues::Selected.get()
                                   : (isActive ? GuiColor::StepValues::Text.get()
                                               : GuiColor::StepValues::DimText.get()));
        }

        const juce::String label = i < (int)labels.size() ? labels[(size_t)i] : juce::String();
        const bool isSkipped = i < (int)skipped.size() && skipped[(size_t)i];
        const juce::String value = isSkipped ? juce::String("SKIP") : juce::String(values[(size_t)i], decimals);

        g.drawFittedText(label.isEmpty() ? value : label + ": " + value, inner.toNearestInt(),
                         juce::Justification::centredLeft, 1, 0.7f);
    }

    g.setColour(GuiColor::StepValues::Frame.get());
    g.drawRect(area, 1.0f);
}
