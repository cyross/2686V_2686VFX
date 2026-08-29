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
#include "../NudgeButtons/NudgeButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"

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
    GuiComponentNudgeSliderFloat r1;
    GuiComponentNudgeButtons r1Nudge;
    GuiComponentNudgeSliderFloat r2;
    GuiComponentNudgeButtons r2Nudge;
    GuiComponentNudgeSliderFloat r3;
    GuiComponentNudgeButtons r3Nudge;
    GuiComponentNudgeSliderFloat r4;
    GuiComponentNudgeButtons r4Nudge;
    GuiComponentNudgeSliderFloat r5;
    GuiComponentNudgeButtons r5Nudge;
    GuiComponentNudgeSliderFloat r6;
    GuiComponentNudgeButtons r6Nudge;
    GuiComponentNudgeSliderFloat r7;
    GuiComponentNudgeButtons r7Nudge;
    GuiComponentNudgeSliderFloat r8;
    GuiComponentNudgeButtons r8Nudge;
    GuiComponentNudgeSliderFloat r9;
    GuiComponentNudgeButtons r9Nudge;
    GuiComponentNudgeSliderFloat r10;
    GuiComponentNudgeButtons r10Nudge;
    GuiComponentNudgeSliderFloat r11;
    GuiComponentNudgeButtons r11Nudge;
    NormalSeparator rateSeparator;
    GuiComponentNudgeSliderFloat startLevel;
    GuiComponentSsgSwButtons stlBtns;
    GuiComponentNudgeSliderFloat l1;
    GuiComponentSsgSwButtons l1Btns;
    GuiComponentNudgeSliderFloat l2;
    GuiComponentSsgSwButtons l2Btns;
    GuiComponentNudgeSliderFloat l3;
    GuiComponentSsgSwButtons l3Btns;
    GuiComponentNudgeSliderFloat l4;
    GuiComponentSsgSwButtons l4Btns;
    GuiComponentNudgeSliderFloat l5;
    GuiComponentSsgSwButtons l5Btns;
    GuiComponentNudgeSliderFloat l6;
    GuiComponentSsgSwButtons l6Btns;
    GuiComponentNudgeSliderFloat l7;
    GuiComponentSsgSwButtons l7Btns;
    GuiComponentNudgeSliderFloat l8;
    GuiComponentSsgSwButtons l8Btns;
    GuiComponentNudgeSliderFloat l9;
    GuiComponentSsgSwButtons l9Btns;
    GuiComponentNudgeSliderFloat l10;
    GuiComponentSsgSwButtons l10Btns;
    GuiComponentNudgeSliderFloat l11;
    GuiComponentSsgSwButtons l11Btns;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // applyLoopValues の入れ子呼び出しを弾くための印。
    // 中で setValue を呼ぶと onValueChange 経由で自分が呼び返される。
    bool isApplyingLoopValues = false;

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
        r1Nudge(context),
        r2(context),
        r2Nudge(context),
        r3(context),
        r3Nudge(context),
        r4(context),
        r4Nudge(context),
        r5(context),
        r5Nudge(context),
        r6(context),
        r6Nudge(context),
		r7(context),
		r7Nudge(context),
		r8(context),
		r8Nudge(context),
		r9(context),
		r9Nudge(context),
		r10(context),
		r10Nudge(context),
		r11(context),
		r11Nudge(context),
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
