#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Curve/GuiCurve.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"
#include "../PitchButtons/PitchButtons.h"
#include "../Separator/NormalSeparator.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentUnison : public GuiBase {
    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiSlider voices;
    GuiSlider detune;
    GuiComponentPitchButtons detuneButtons;
    GuiSlider spread;
    NormalSeparator arpSeparator;
    GuiToggleButton arpEnable;
    GuiSlider arpFreq;
    GuiToggleButton arpSmooth;
    NormalSeparator paraSeparator;

    // ボイス単位の設定。1組のスライダーを targetVoice で切り替えて使う
    GuiSlider targetVoice;
    GuiSlider paraDistance;
    GuiSlider paraDetune;
    GuiComponentPitchButtons paraDetuneButtons;
    juce::String paramCode;

    void rebindParaSliders();
    float getParaValue(const juce::String& key, int voiceIndex);
    void setParaValue(const juce::String& key, int voiceIndex, float value);

    std::unique_ptr<juce::FileChooser> fileChooser;

public:
    GuiComponentUnison(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        voices(context),
        detune(context),
        detuneButtons(context),
        spread(context),
        arpSeparator(context),
        arpEnable(context),
        arpFreq(context),
        arpSmooth(context),
        paraSeparator(context),
        targetVoice(context),
        paraDistance(context),
        paraDetune(context),
        paraDetuneButtons(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void copyParams(CopyUnison& copyObj);
    void pasteParams(CopyUnison& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
