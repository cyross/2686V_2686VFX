#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiAdpcm : public GuiBase
{
    // --- ADPCM Page ---
    GuiGroup mainGroup;

    GuiComboBox modeSelector;

    GuiTextButton loadButton;
    GuiTextButton clearButton;
    GuiLabel fileNameLabel;
    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;

    GuiSlider levelSlider;
    GuiSlider panSlider;

    GuiTextButton btnPanL;
    GuiTextButton btnPanC;
    GuiTextButton btnPanR;

    // Loop Button
    GuiToggleButton loopButton;

    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel mainCat;
    GuiCategoryLabel panCat;
    GuiCategoryLabel adsrCat;

    GuiToggleButton adsrBypassButton; // ADSR Bypass Switch
    GuiSlider attackSlider;
    GuiSlider decaySlider;
    GuiSlider sustainSlider;
    GuiSlider releaseSlider;

    GuiComboBox rateSelector;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
public:
    GuiAdpcm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        mvolCat(context),
        masterVolSlider(context),
        modeSelector(context),
        loadButton(context),
        clearButton(context),
        fileNameLabel(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        levelSlider(context),
        panSlider(context),
        btnPanL(context),
        btnPanC(context),
        btnPanR(context),
        loopButton(context),
        presetNameCat(context),
        qualityCat(context),
        mainCat(context),
        panCat(context),
        adsrCat(context),
		adsrBypassButton(context),
        attackSlider(context),
        decaySlider(context),
        sustainSlider(context),
        releaseSlider(context),
        rateSelector(context),
        presetNameLabel(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateFileName(const juce::String& fileName);
    bool isThis(juce::Button* button);
    bool isBtnPanL(juce::Button* button);
    bool isBtnPanC(juce::Button* button);
    bool isBtnPanR(juce::Button* button);
    void setPan(float pan);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void updatePresetName(const juce::String& presetName);
};
