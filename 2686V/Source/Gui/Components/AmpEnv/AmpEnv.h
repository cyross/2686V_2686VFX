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

class GuiComponentAmpEnv : public GuiBase {
    // AMP ENV
    GuiCategoryLabel cat;
    GuiToggleButton bypass;
    GuiSlider startLevel;
    GuiSlider attack;
    GuiSlider decay;
    GuiSlider sustain;
    GuiSlider release;
    GuiToggleButton kor;
    std::unique_ptr<juce::FileChooser> fileChooser;

public:
    GuiComponentAmpEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        bypass(context),
        attack(context),
        decay(context),
        sustain(context),
        release(context),
        startLevel(context),
        kor(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvAmpAdsr& copyObj);
    void pasteParams(CopyEnvAmpAdsr& copyObj);
    void importParams();
    void exportParams();
};
