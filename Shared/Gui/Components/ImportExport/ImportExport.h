#pragma once

#include <functional>

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

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

        // 区分の見出しを持つ部品なら、右クリックの IMPORT / EXPORT もつなぐ
        if constexpr (requires { target.category(); }) bindCategory(target.category());
    }

    // 区分の見出しの右クリックの「読み込み (IMPORT)」「書き出し (EXPORT)」を、
    // このボタンと同じ処理へつなぐ。押したときに onClickImport / onClickExport を
    // 引くので、あとから差し替えてもかまわない。
    void bindCategory(GuiCategoryLabel& label) {
        label.onImport = [this] { if (onClickImport != nullptr) onClickImport(); };
        label.onExport = [this] { if (onClickExport != nullptr) onClickExport(); };
    }

    void layoutComponent(juce::Rectangle<int>& rect);
    void setVisible(bool visible);
    void setEnable(bool enable);
};
