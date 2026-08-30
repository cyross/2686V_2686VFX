#include "./NormalSeparator.h"

#include "../../../Core/Gui/GuiHelpers.h"

void NormalSeparator::setupComponent(juce::Component& parent) {
    parent.addAndMakeVisible(*this);

    this->setup({ .lineThick = (float)thick, .lineColour = GuiColor::Separator::Line });
    this->setEnabled(true);
}

void NormalSeparator::layoutComponent(juce::Rectangle<int>& rect) {
	rect.removeFromTop(paddingTop);

    auto separatorArea = rect.removeFromTop(thick);

    this->setBounds(separatorArea);

    rect.removeFromTop(paddingBottom);
}

void NormalSeparator::layoutComponentBottom(juce::Rectangle<int>& rect) {
    rect.removeFromBottom(paddingBottom);

    auto separatorArea = rect.removeFromBottom(thick);

    this->setBounds(separatorArea);

    rect.removeFromBottom(paddingTop);
}
