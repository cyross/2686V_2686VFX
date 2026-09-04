#include "./StepValues.h"

void GuiStepValues::paint(juce::Graphics& g)
{
    const int count = (int)values.size();

    if (count <= 0) return;

    auto area = getLocalBounds().toFloat();

    g.setColour(GuiColor::StepValues::Bg.get());
    g.fillRect(area);

    // 桁が増えても崩れないよう、幅は段の数で等分する。
    const float cellW = area.getWidth() / (float)count;

    juce::Font font(juce::FontOptions(juce::jmin(11.0f, area.getHeight() * 0.45f)));

    for (int i = 0; i < count; ++i)
    {
        auto cell = juce::Rectangle<float>(area.getX() + cellW * (float)i, area.getY(), cellW, area.getHeight());

        const bool isSelected = (i == selected);
        const bool isActive = (activeCount < 0) || (i < activeCount);

        if (isSelected)
        {
            g.setColour(GuiColor::StepValues::SelectedBg.get());
            g.fillRect(cell.reduced(0.5f));
        }

        g.setColour(isSelected ? GuiColor::StepValues::Selected.get()
                               : (isActive ? GuiColor::StepValues::Text.get()
                                           : GuiColor::StepValues::DimText.get()));

        g.setFont(font);

        auto upper = cell.removeFromTop(cell.getHeight() * 0.5f);

        if (i < (int)labels.size()) {
            g.drawFittedText(labels[(size_t)i], upper.toNearestInt(), juce::Justification::centred, 1, 0.7f);
        }

        g.drawFittedText(juce::String(values[(size_t)i], decimals), cell.toNearestInt(),
                         juce::Justification::centred, 1, 0.7f);
    }

    g.setColour(GuiColor::StepValues::Frame.get());
    g.drawRect(area, 1.0f);
}
