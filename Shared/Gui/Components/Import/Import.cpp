#include "./Import.h"

#include "Shared/Core/Gui/GuiHelpers.h"

void GuiComponentImport::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FileButton::ImportBg, .isReset = false, .isResized = false});
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };
}

void GuiComponentImport::setupComponentOp(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FileButton::ImportOpBg, .isReset = false, .isResized = false });
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
