#include "./ShortSeparator.h"

#include "../../../Core/Gui/GuiHelpers.h"

void ShortSeparator::setupComponent(juce::Component& parent) {
    parent.addAndMakeVisible(*this);

    this->setup({ .lineRate = widthRate, .lineThick = (float)thick, .lineColour = juce::Colours::white });
    this->setEnabled(true);
}

void ShortSeparator::layoutComponent(juce::Rectangle<int>& rect) {
    rect.removeFromTop(paddingTop);

    auto separatorArea = rect.removeFromTop(thick);

    this->setBounds(separatorArea);

    rect.removeFromTop(paddingBottom);
}

void ShortSeparator::layoutComponentBottom(juce::Rectangle<int>& rect) {
    rect.removeFromBottom(paddingBottom);

    auto separatorArea = rect.removeFromBottom(thick);

    this->setBounds(separatorArea);

    rect.removeFromBottom(paddingTop);
}
