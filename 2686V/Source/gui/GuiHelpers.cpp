#include "GuiHelpers.h"

#include "../core/MmlConstants.h"

#include "../fm/RegisterType.h"
#include "../fm/RegisterConverter.h"

std::vector<SelectItem> createItems(int size, const juce::String& prefix)
{
    std::vector<SelectItem> items;

    for (int i = 0; i < size; ++i) items.push_back({ .name = prefix + juce::String(i), .value = i + 1 });

    return items;
}

std::vector<SelectItem> createAlgItems(int size)
{
    return createItems(size, "Alg ");
}

// Helper function to get Note Name (Cubase Standard: C3 = 60)
juce::String getNoteName(int noteNumber)
{
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (noteNumber / 12) - 2; // MIDI 0 = C-2, MIDI 36 = C1, MIDI 60 = C3
    int noteIndex = noteNumber % 12;
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

void layoutComponentsLtoR(juce::Rectangle<int>& rect,
    int height,
    int space,
    std::initializer_list<std::pair<juce::Component*, std::pair<int, int>>> components)
{
    auto area = rect.removeFromTop(height);

    for (const auto& comp : components)
    {
        if (comp.first != nullptr) // 安全のためのチェック
        {
            comp.first->setBounds(area.removeFromLeft(comp.second.first));
            area.removeFromLeft(comp.second.second);
        }
    }

    rect.removeFromTop(space);
}
