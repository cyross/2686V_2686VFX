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
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "../PitchButtons/PitchButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentSsgSwPEnv11 : public GuiBase {
    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    // SSG SW ENV
    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
	NormalSeparator flagSeparator;
    GuiSlider steps;
	NormalSeparator stepsSeparator;
    GuiToggleButton loop;
    GuiSlider loopTo;
    GuiSlider loopCount;
	NormalSeparator loopSeparator;
    GuiSlider r1;
    GuiSlider r2;
    GuiSlider r3;
    GuiSlider r4;
    GuiSlider r5;
    GuiSlider r6;
    GuiSlider r7;
    GuiSlider r8;
    GuiSlider r9;
    GuiSlider r10;
    GuiSlider r11;
    NormalSeparator rateSeparator;
    GuiSlider startLevel;
    GuiComponentPitchButtons startLevelButtons;
    GuiSlider l1;
    GuiComponentPitchButtons l1Buttons;
    GuiSlider l2;
    GuiComponentPitchButtons l2Buttons;
    GuiSlider l3;
    GuiComponentPitchButtons l3Buttons;
    GuiSlider l4;
    GuiComponentPitchButtons l4Buttons;
    GuiSlider l5;
    GuiComponentPitchButtons l5Buttons;
    GuiSlider l6;
    GuiComponentPitchButtons l6Buttons;
    GuiSlider l7;
    GuiComponentPitchButtons l7Buttons;
    GuiSlider l8;
    GuiComponentPitchButtons l8Buttons;
    GuiSlider l9;
    GuiComponentPitchButtons l9Buttons;
    GuiSlider l10;
    GuiComponentPitchButtons l10Buttons;
    GuiSlider l11;
    GuiComponentPitchButtons l11Buttons;
    std::unique_ptr<juce::FileChooser> fileChooser;

    void applyLoopValues(bool enabled);
public:
    GuiComponentSsgSwPEnv11(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        flag(context),
		flagSeparator(context),
		steps(context),
		stepsSeparator(context),
		loop(context),
		loopTo(context), 
		loopCount(context),
		loopSeparator(context),
        r1(context),
        r2(context),
        r3(context),
        r4(context),
        r5(context),
        r6(context),
        r7(context),
        r8(context),
        r9(context),
        r10(context),
        r11(context),
        rateSeparator(context),
        startLevel(context),
        startLevelButtons(context),
		l1(context),
        l1Buttons(context),
		l2(context),
		l2Buttons(context),
		l3(context),
		l3Buttons(context),
		l4(context),
		l4Buttons(context),
		l5(context),
		l5Buttons(context),
		l6(context),
		l6Buttons(context),
        l7(context),
        l7Buttons(context),
        l8(context),
		l8Buttons(context),
        l9(context),
		l9Buttons(context),
        l10(context),
		l10Buttons(context),
        l11(context),
		l11Buttons(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx);
    void setEnabled(bool enabled);
    void copyParams(CopyPEnvSsgSw11& copyObj);
    void pasteParams(CopyPEnvSsgSw11& copyObj);
    void importParams();
    void exportParams();
};
