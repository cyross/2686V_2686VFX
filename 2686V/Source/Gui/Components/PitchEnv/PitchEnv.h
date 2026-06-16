#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <functional>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Curve/GuiCurve.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"
#include "../PitchButtons/PitchButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentPitchEnv : public GuiBase {
    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(6.0f));

    // PITCH ENV
    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
    GuiSlider attack;
    GuiSlider decay;
    GuiSlider release;
    GuiSlider startLevel;
    GuiComponentPitchButtons startLevelButtons;
    GuiSlider attackLevel;
    GuiComponentPitchButtons attackLevelButtons;
    GuiSlider sustainLevel;
    GuiComponentPitchButtons sustainLevelButtons;
    GuiSlider releaseLevel;
    GuiComponentPitchButtons releaseLevelButtons;

public:
    GuiComponentPitchEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        flag(context),
        attack(context),
        decay(context),
        release(context),
        startLevel(context),
        startLevelButtons(context),
        attackLevel(context),
        attackLevelButtons(context),
        sustainLevel(context),
        sustainLevelButtons(context),
        releaseLevel(context),
        releaseLevelButtons(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvPitchAdsr& copyObj);
    void pasteParams(CopyEnvPitchAdsr& copyObj);
};
