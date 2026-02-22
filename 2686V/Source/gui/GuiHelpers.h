#pragma once

#include <JuceHeader.h>
#include <vector>
#include "GuiComponents.h"
#include "GuiBase.h"

std::vector<SelectItem> createItems(int size, const juce::String& prefix);
std::vector<SelectItem> createAlgItems(int size);
juce::String getNoteName(int noteNumber);
void layoutComponentsLtoR(juce::Rectangle<int>& rect,
    int height,
    int space,
    std::initializer_list<std::pair<juce::Component*, std::pair<int, int>>> components);
