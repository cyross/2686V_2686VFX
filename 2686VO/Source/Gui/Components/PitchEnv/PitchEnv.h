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
#include "../PitchButtons/PitchButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentPitchEnv : public GuiBase {
    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    // PITCH ENV
    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
    NormalSeparator flagSeparator;
    GuiSlider attack;
    GuiSlider decay;
    GuiSlider release;
    NormalSeparator rateSeparator;
    GuiSlider startLevel;
    GuiComponentPitchButtons startLevelButtons;
    GuiSlider attackLevel;
    GuiComponentPitchButtons attackLevelButtons;
    GuiSlider sustainLevel;
    GuiComponentPitchButtons sustainLevelButtons;
    GuiSlider releaseLevel;
    GuiComponentPitchButtons releaseLevelButtons;
    std::unique_ptr<juce::FileChooser> fileChooser;

public:
    GuiComponentPitchEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        flag(context),
        flagSeparator(context),
        attack(context),
        decay(context),
        release(context),
		rateSeparator(context),
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
    void updateGraph(GuiEnvelopeGraph& graph);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvPitchAdsr& copyObj);
    void pasteParams(CopyEnvPitchAdsr& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
