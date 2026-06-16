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

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentSsgSwEnv : public GuiBase {
    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(6.0f));

    // SSG SW ENV
    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
    GuiSlider steps;
    GuiToggleButton loop;
    GuiSlider loopTo;
    GuiSlider loopCount;
    GuiSlider startLevel;
    GuiSlider r1;
    GuiSlider l1;
    GuiSlider r2;
    GuiSlider l2;
    GuiSlider r3;
    GuiSlider l3;
    GuiSlider r4;
    GuiSlider l4;
    GuiSlider r5;
    GuiSlider l5;
    GuiSlider r6;
    GuiSlider l6;

    void applyLoopValues(bool enabled);
public:
    GuiComponentSsgSwEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        flag(context),
		steps(context),
		loop(context),
		loopTo(context), 
		loopCount(context),
		startLevel(context),
		r1(context),
		l1(context),
		r2(context),
		l2(context),
		r3(context),
		l3(context),
		r4(context),
		l4(context),
		r5(context),
		l5(context),
		r6(context),
		l6(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvSsgSw& copyObj);
    void pasteParams(CopyEnvSsgSw& copyObj);
};
