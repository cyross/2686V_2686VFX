#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

class NormalSeparator : public GuiSeparator {
public:
    static constexpr int paddingTop = 6;
    static constexpr int paddingBottom = 6;
    static constexpr int thick = 4;

    NormalSeparator(const GuiContext& context) :
        GuiSeparator(context)
    {
    }

    void setupComponent(juce::Component& parent);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentBottom(juce::Rectangle<int>& rect);
    static const int getHeight() { return paddingTop + thick + paddingBottom; }
};
