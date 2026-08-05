#pragma once

#include <functional>

#include <JuceHeader.h>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

class GuiComponentImportExport : public GuiBase {
    GuiTextButton importButton;
    GuiTextButton exportButton;
public:
    GuiComponentImportExport(const GuiContext& context) :
        GuiBase(context),
        importButton(context),
        exportButton(context)
    {
    }

    std::function<void()> onClickImport = nullptr;
    std::function<void()> onClickExport = nullptr;

    void setupComponent(juce::Component& parent, int& tabOrder, const juce::String& title);
    void setupComponentOp(juce::Component& parent, int& tabOrder, const juce::String& title);
    void layoutComponent(juce::Rectangle<int>& rect);
    void setVisible(bool visible);
};
