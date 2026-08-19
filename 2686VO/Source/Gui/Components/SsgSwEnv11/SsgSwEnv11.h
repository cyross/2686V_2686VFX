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
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "../../../Gui/Components/SsgSwButtons/SsgSwButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentSsgSwEnv11 : public GuiBase {
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
    GuiComponentSsgSwButtons stlBtns;
    GuiSlider l1;
    GuiComponentSsgSwButtons l1Btns;
    GuiSlider l2;
    GuiComponentSsgSwButtons l2Btns;
    GuiSlider l3;
    GuiComponentSsgSwButtons l3Btns;
    GuiSlider l4;
    GuiComponentSsgSwButtons l4Btns;
    GuiSlider l5;
    GuiComponentSsgSwButtons l5Btns;
    GuiSlider l6;
    GuiComponentSsgSwButtons l6Btns;
    GuiSlider l7;
    GuiComponentSsgSwButtons l7Btns;
    GuiSlider l8;
    GuiComponentSsgSwButtons l8Btns;
    GuiSlider l9;
    GuiComponentSsgSwButtons l9Btns;
    GuiSlider l10;
    GuiComponentSsgSwButtons l10Btns;
    GuiSlider l11;
    GuiComponentSsgSwButtons l11Btns;
    std::unique_ptr<juce::FileChooser> fileChooser;

    void applyLoopValues(bool enabled);
public:
    GuiComponentSsgSwEnv11(const GuiContext& context) :
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
        stlBtns(context),
		l1(context),
        l1Btns(context),
        l2(context),
        l2Btns(context),
        l3(context),
        l3Btns(context),
        l4(context),
        l4Btns(context),
        l5(context),
        l5Btns(context),
        l6(context),
        l6Btns(context),
        l7(context),
        l7Btns(context),
        l8(context),
        l8Btns(context),
        l9(context),
        l9Btns(context),
        l10(context),
        l10Btns(context),
        l11(context),
        l11Btns(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvSsgSw11& copyObj);
    void pasteParams(CopyEnvSsgSw11& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
