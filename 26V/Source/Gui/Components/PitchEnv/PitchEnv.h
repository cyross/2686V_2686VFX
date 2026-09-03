#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
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
#include "../NudgeButtons/NudgeButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentPitchEnv : public GuiBase {
    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    // PITCH ENV
    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
    NormalSeparator flagSeparator;
    GuiComponentNudgeSliderFloat attack;
    GuiComponentNudgeButtons attackNudge;
    GuiComponentNudgeSliderFloat decay;
    GuiComponentNudgeButtons decayNudge;
    GuiComponentNudgeSliderFloat release;
    GuiComponentNudgeButtons releaseNudge;
    NormalSeparator rateSeparator;
    GuiComponentNudgeSliderFloat startLevel;
    GuiComponentPitchButtons startLevelButtons;
    GuiComponentNudgeSliderFloat attackLevel;
    GuiComponentPitchButtons attackLevelButtons;
    GuiComponentNudgeSliderFloat sustainLevel;
    GuiComponentPitchButtons sustainLevelButtons;
    GuiComponentNudgeSliderFloat releaseLevel;
    GuiComponentPitchButtons releaseLevelButtons;
    std::unique_ptr<juce::FileChooser> fileChooser;

public:

    // 簡易表示モードで丸ごと隠す。見出しごと消え、縦の場所も取らない。
    void setCategoryVisible(bool visible) { cat.setHidden(!visible); }
    GuiComponentPitchEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        flag(context),
        flagSeparator(context),
        attack(context),
        attackNudge(context),
        decay(context),
        decayNudge(context),
        release(context),
        releaseNudge(context),
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

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
