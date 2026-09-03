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

class GuiComponentSsgSwPEnv11 : public GuiBase {
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
    GuiComponentPitchButtons startLevelButtons;
    GuiComponentNudgeSliderFloat l1;
    GuiComponentPitchButtons l1Buttons;
    GuiComponentNudgeSliderFloat l2;
    GuiComponentPitchButtons l2Buttons;
    GuiComponentNudgeSliderFloat l3;
    GuiComponentPitchButtons l3Buttons;
    GuiComponentNudgeSliderFloat l4;
    GuiComponentPitchButtons l4Buttons;
    GuiComponentNudgeSliderFloat l5;
    GuiComponentPitchButtons l5Buttons;
    GuiComponentNudgeSliderFloat l6;
    GuiComponentPitchButtons l6Buttons;
    GuiComponentNudgeSliderFloat l7;
    GuiComponentPitchButtons l7Buttons;
    GuiComponentNudgeSliderFloat l8;
    GuiComponentPitchButtons l8Buttons;
    GuiComponentNudgeSliderFloat l9;
    GuiComponentPitchButtons l9Buttons;
    GuiComponentNudgeSliderFloat l10;
    GuiComponentPitchButtons l10Buttons;
    GuiComponentNudgeSliderFloat l11;
    GuiComponentPitchButtons l11Buttons;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // applyLoopValues の入れ子呼び出しを弾くための印。
    // 中で setValue を呼ぶと onValueChange 経由で自分が呼び返される。
    bool isApplyingLoopValues = false;

    void applyLoopValues(bool enabled);
public:

    // 簡易表示モードで丸ごと隠す。見出しごと消え、縦の場所も取らない。
    //
    // 見出しを見せるかどうかはレイアウト側では戻らない (あちらは場所を
    // 決めるだけ) ので、ここで両方向とも面倒を見る。
    void setCategoryVisible(bool visible) {
        cat.setHidden(!visible);
        cat.setVisible(visible);
    }

    // 簡易表示モードの一括操作で使う口。
    //
    // 区分によって「バイパス」だったり「有効」だったりするので、
    // ここで意味を揃えて「切ってあるか」で答える。
    bool hasBypassSwitch() const { return true; }

    bool isCategoryBypassed() const { return isEnable ? !flag.getToggleState() : flag.getToggleState(); }

    void setCategoryBypassed(bool bypassed) {
        flag.setToggleState(isEnable ? !bypassed : bypassed, juce::sendNotification);
    }

    // 見出しの開閉
    void setCategoryOpen(bool open) { cat.setDetailVisible(open); }
    GuiComponentSsgSwPEnv11(const GuiContext& context) :
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
        startLevelButtons(context),
		l1(context),
        l1Buttons(context),
		l2(context),
		l2Buttons(context),
		l3(context),
		l3Buttons(context),
		l4(context),
		l4Buttons(context),
		l5(context),
		l5Buttons(context),
		l6(context),
		l6Buttons(context),
        l7(context),
        l7Buttons(context),
        l8(context),
		l8Buttons(context),
        l9(context),
		l9Buttons(context),
        l10(context),
		l10Buttons(context),
        l11(context),
		l11Buttons(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph);
    void setEnabled(bool enabled);
    void copyParams(CopyPEnvSsgSw11& copyObj);
    void pasteParams(CopyPEnvSsgSw11& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
