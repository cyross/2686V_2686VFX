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
#include "../WavePreview/WavePreview.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentLfoOpzx7 : public GuiBase {
    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(6.0f));

    // OPZX7 LFO
    GuiCategoryLabel cat;
    GuiLabel pmLabel;
    GuiToggleButton pmEnable;
    GuiSlider pmFreq;
    GuiSlider pmSyncDelay;
    GuiTextButton pmSDToZero;
    GuiTextButton pmSDToOne;
    GuiComboBox pgShape;
    GuiWavePreview pmPreview;
    GuiSlider pms;
    GuiSlider pmd;
    NormalSeparator pmAmSeparator;
    GuiLabel amLabel;
    GuiToggleButton amEnable;
    GuiSlider amFreq;
    GuiSlider amSyncDelay;
    GuiTextButton amSDToZero;
    GuiTextButton amSDToOne;
    GuiComboBox egShape;
    GuiSlider amSmRt;
    GuiWavePreview amPreview;
    GuiSlider ams;
    GuiSlider amd;
    std::unique_ptr<juce::FileChooser> fileChooser;
public:
    GuiComponentLfoOpzx7(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        pmLabel(context),
        pmEnable(context),
        pmFreq(context),
        pmSyncDelay(context),
        pmSDToZero(context),
        pmSDToOne(context),
        pgShape(context),
        pmPreview(context),
        pms(context),
        pmd(context),
        pmAmSeparator(context),
        amLabel(context),
        amEnable(context),
        amFreq(context),
        amSyncDelay(context),
        amSDToZero(context),
        amSDToOne(context),
        egShape(context),
        amSmRt(context),
        amPreview(context),
        ams(context),
        amd(context)
    {
    }

    void setupComponent(
        juce::Component& parent,
        const juce::String& code,
        int& tabOrder
    );
    void layoutComponent(juce::Rectangle<int>& rect);
    void updatePreviews();
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnabled(bool enabled);
    void copyParams(CopyLfoOpzx7& copyObj);
    void pasteParams(CopyLfoOpzx7& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
