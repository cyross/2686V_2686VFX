#include "./ImportExport.h"

#include "../../../Core/Gui/GuiHelpers.h"

void GuiComponentImportExport::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false});
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };

    exportButton.setup({ .parent = parent, .title = juce::String("") + "[EX]" + title, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportButton.setWantsKeyboardFocus(true);
    exportButton.setExplicitFocusOrder(++tabOrder);
    exportButton.onClick = [this] {
        if (onClickExport != nullptr) {
            onClickExport();
        }
        };
}

void GuiComponentImportExport::setupComponentOp(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };

    exportButton.setup({ .parent = parent, .title = juce::String("") + "[EX]" + title, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportButton.setWantsKeyboardFocus(true);
    exportButton.setExplicitFocusOrder(++tabOrder);
    exportButton.onClick = [this] {
        if (onClickExport != nullptr) {
            onClickExport();
        }
        };
}

void GuiComponentImportExport::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMainTwoComps({ .rect = rect, .comp1 = &importButton, .comp2 = &exportButton });
}

void GuiComponentImportExport::setVisible(bool visible) {
    importButton.setVisible(visible);
    exportButton.setVisible(visible);
}
