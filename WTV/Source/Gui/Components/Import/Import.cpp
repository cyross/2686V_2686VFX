#include "./Import.h"

#include "../../../Core/Gui/GuiHelpers.h"

void GuiComponentImport::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false});
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };
}

void GuiComponentImport::setupComponentOp(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };
}

void GuiComponentImport::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMain({ .mainRect = rect, .component = &importButton });
}

void GuiComponentImport::setVisible(bool visible) {
    importButton.setVisible(visible);
}

void GuiComponentImport::setEnable(bool enable) {
    importButton.setEnabled(enable);
}
