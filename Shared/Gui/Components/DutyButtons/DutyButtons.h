#pragma once

#include <JuceHeader.h>
#include <array>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// デューティ比を直に置くボタン (DUTY RATIO ENV)
// ============================================================================
// SSG の矩形波の MODE で選べるデューティ比 (PRESET の 9 つ) を、そのまま
// % で置く。VAR は 0〜50% の連続値なので、つまみで合わせる。
class GuiComponentDutyButtons : public GuiBase {
public:
    // 並びは SSG の PRESET と同じ (SsgCore::dutyPresets)
    static constexpr std::array<float, 9> percents = { 50.0f, 43.75f, 37.5f, 31.25f, 25.0f, 20.0f, 18.75f, 12.5f, 6.25f };

    explicit GuiComponentDutyButtons(const GuiContext& context);

    void setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font = std::nullopt);
    void layoutComponent(juce::Rectangle<int>& rect, int height = 15);
    void setVisibles(bool visible);
    void setEnables(bool enabled);

private:
    std::array<std::unique_ptr<GuiTextButton>, percents.size()> buttons;
};
