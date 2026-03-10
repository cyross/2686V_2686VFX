#pragma once

#include <JuceHeader.h>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"

class GuiBeep : public GuiBase {
    GuiGroup mainGroup;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel mainCat;

    GuiSlider volSlider;

    GuiCategoryLabel catFix;

    GuiToggleButton fixToggle;
    GuiSlider freqSlider;
    GuiTextButton freqTo2kBtn;

    GuiCategoryLabel adsrCat;

    GuiToggleButton bypassToggle;
    GuiSlider ar, dr, sl, rr;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
public:
    GuiBeep(const GuiContext& context) : GuiBase(context),
        mainGroup(context),
        monoPolyCat(context),
        presetNameCat(context),
        mainCat(context),
        volSlider(context),
        catFix(context),
        fixToggle(context), freqSlider(context), freqTo2kBtn(context),
        adsrCat(context),
        bypassToggle(context),
        ar(context), dr(context), sl(context), rr(context),
        mvolCat(context),
        masterVolSlider(context),
        monoModeToggle(context),
        presetNameLabel(context)
    {
    }
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& presetName);
};
