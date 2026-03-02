#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class RhythmPadGui: public GuiBase
{
    GuiGroup mainGroup;

    GuiLabel fileNameLabel;

    GuiTextButton loadButton;

    GuiTextButton clearButton;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel mainCat;

    GuiSlider noteSlider;

    GuiComboBox modeSelector;

    GuiComboBox rateSelector;

    GuiSlider panSlider;

    GuiTextButton btnPanL;
    GuiTextButton btnPanC;
    GuiTextButton btnPanR;

    GuiSlider volSlider;

    GuiToggleButton oneShotButton;

    GuiCategoryLabel adsrCat;

    GuiSlider rrSlider;
public:
    RhythmPadGui(const GuiContext& context) :
		GuiBase(context),
        mainGroup(context),
        fileNameLabel(context),
        loadButton(context),
        clearButton(context),
        qualityCat(context),
        mainCat(context),
        noteSlider(context),
        modeSelector(context),
        rateSelector(context),
        panSlider(context),
        btnPanL(context),
        btnPanC(context),
        btnPanR(context),
        volSlider(context),
        oneShotButton(context),
        adsrCat(context),
        rrSlider(context)
	{
	}

    void updatePadFileName(const juce::String& fileName);
    void setup(juce::Component& parent, int index, juce::String padName);
	void layout(juce::Rectangle<int> content);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    bool isThis(juce::Button* button);
};

class GuiRhythm : public GuiBase
{
    GuiGroup mainGroup;

    GuiCategoryLabel mainCat;

    // Master Level
	GuiSlider levelSlider;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // 8 Pads
    std::array<RhythmPadGui, 8> pads;
public:
	GuiRhythm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        mainCat(context),
        levelSlider(context),
        mvolCat(context),
        masterVolSlider(context),
        pads{ { {context}, {context}, {context}, {context}, {context}, {context}, {context}, {context} } }
    {}
                     
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void buttonClicked(juce::Button* button, juce::AudioFormatManager &formatManager, std::unique_ptr<juce::FileChooser>& fileChooser);
	void updatePadFileName(int padIndex, const juce::String& fileName);
    bool isThis(int padIndex, juce::Button* button);
};
