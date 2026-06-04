#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <functional>

#include "../../../Core/Const/Global.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Curve/Core.h"
#include "../../../Advanced/Curve/Core.h"


class GuiComponentPitchEnv : public GuiBase {
    bool isEnable = false;

    // PITCH ENV
    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
    GuiSlider attack;
    GuiSlider decay;
    GuiSlider release;
    GuiSlider startLevel;
    GuiSlider attackLevel;
    GuiSlider sustainLevel;
    GuiSlider releaseLevel;

public:
    GuiComponentPitchEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        flag(context),
        attack(context),
        decay(context),
        release(context),
        startLevel(context),
        attackLevel(context),
        sustainLevel(context),
        releaseLevel(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx);
    void setEnabled(bool enabled);
};
