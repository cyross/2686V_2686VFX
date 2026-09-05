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

    // importParams()/exportParams() をそのまま呼ぶだけの場合はこちらを使う
    template <typename T>
    void setupComponentFor(juce::Component& parent, int& tabOrder, const juce::String& title, T& target) {
        setupComponent(parent, tabOrder, title);
        onClickImport = [&target] { target.importParams(); };
        onClickExport = [&target] { target.exportParams(); };
    }

    void layoutComponent(juce::Rectangle<int>& rect);
    void setVisible(bool visible);
    void setEnable(bool enable);
};
