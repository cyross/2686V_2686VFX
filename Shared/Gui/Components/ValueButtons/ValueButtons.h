#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// 値を直に置く・足すボタンの並び
// ============================================================================
// 256 段のエンベロープ (GuiComponentStepEnv) の値のつまみの下に置く。
// DUTY RATIO ENV は MODE で選べるデューティ比を置き、TL ENV は TL を
// 少しずつ動かす。1 行に 5 個まで並べ、余れば次の行へ送る。
class GuiComponentValueButtons : public GuiBase {
public:
    struct Entry {
        juce::String label;

        // true なら今の値へ足す。false なら value を置く
        bool add = false;
        double value = 0.0;

        // true なら value に「1 刻み」(setStepSize で渡した幅) を掛けて足す (PARAM ENV)
        bool step = false;
    };

    // 1 行に並べる数の既定
    static constexpr int defaultPerRow = 5;

    explicit GuiComponentValueButtons(const GuiContext& context) : GuiBase(context) {}

    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, const std::vector<Entry>& entries, std::optional<juce::Font> font = std::nullopt, int perRow = defaultPerRow);
    void layoutComponent(juce::Rectangle<int>& rect, int height = 15);
    void setVisibles(bool visible);
    void setEnables(bool enabled);

    // step の付いたボタンの 1 刻みの幅。押すたびに引く (指し先で変わるため)
    void setStepSize(std::function<double()> size) { stepSize = std::move(size); }

private:
    std::function<double()> stepSize;

    std::vector<std::unique_ptr<GuiTextButton>> buttons;
    int perRow = defaultPerRow;
};
