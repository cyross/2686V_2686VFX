#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

class ShortSeparator : public GuiSeparator {
public:
    static constexpr int paddingTop = 6;
    static constexpr int paddingBottom = 6;
    static constexpr int thick = 2;
    static constexpr float widthRate = 0.8f;

    ShortSeparator(const GuiContext& context) :
        GuiSeparator(context)
    {
    }

    void setupComponent(juce::Component& parent);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentBottom(juce::Rectangle<int>& rect);
    static const int getHeight() { return paddingTop + thick + paddingBottom; }
};
