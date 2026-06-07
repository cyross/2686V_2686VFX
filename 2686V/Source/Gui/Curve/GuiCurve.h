#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Processor/Curve/ProcessorCurveValues.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "./GuiCurveGraph.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiCurve : public GuiBase
{
    // Pane Groups
    GuiGroup curveGroup;

    std::array<std::unique_ptr<GuiCurveGraph>, CurvePrValue::params> curveGraphs;

    void updateEnabled();
    void updateVisible();
public:
    GuiCurve(const GuiContext& context);

    GuiToggleButton enable;
    GuiComboBox position;
    GuiComboBox target;
    GuiSeparator mainSeparator;

    std::array<std::unique_ptr<GuiLabel>, CurvePrValue::params> paramLabel;

    std::array<std::array<std::array<std::unique_ptr<GuiComboBox>, CurvePrValue::params>, CurvePrValue::targets>, CurvePrValue::positions> logic;
    std::array<std::array<std::array<std::unique_ptr<GuiSlider>, CurvePrValue::params>, CurvePrValue::targets>, CurvePrValue::positions> k;
    std::array<std::array<std::array<std::array<std::unique_ptr<GuiSlider>, CurvePrValue::values>, CurvePrValue::params>, CurvePrValue::targets>, CurvePrValue::positions> value;

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void initParams();
};
