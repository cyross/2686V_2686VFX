#include "./ImportExport.h"

#include "Shared/Core/Gui/GuiHelpers.h"

void GuiComponentImportExport::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FileButton::ImportBg, .isReset = false, .isResized = false});
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };

    exportButton.setup({ .parent = parent, .title = juce::String("") + "[EX]" + title, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FileButton::ExportBg, .isReset = false, .isResized = false });
    exportButton.setWantsKeyboardFocus(true);
    exportButton.setExplicitFocusOrder(++tabOrder);
    exportButton.onClick = [this] {
        if (onClickExport != nullptr) {
            onClickExport();
        }
        };
}

void GuiComponentImportExport::setupComponentOp(juce::Component& parent, int& tabOrder, const juce::String& title) {
    importButton.setup({ .parent = parent, .title = juce::String("") + "[IM]" + title, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FileButton::ImportOpBg, .isReset = false, .isResized = false });
    importButton.setWantsKeyboardFocus(true);
    importButton.setExplicitFocusOrder(++tabOrder);
    importButton.onClick = [this] {
        if (onClickImport != nullptr) {
            onClickImport();
        }
        };

    exportButton.setup({ .parent = parent, .title = juce::String("") + "[EX]" + title, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FileButton::ExportOpBg, .isReset = false, .isResized = false });
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

void GuiComponentImportExport::setEnable(bool enable) {
    importButton.setEnabled(enable);
    exportButton.setEnabled(enable);
}
