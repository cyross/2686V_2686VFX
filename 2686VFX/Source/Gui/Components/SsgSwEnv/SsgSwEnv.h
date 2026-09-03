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
#include "../../../Gui/Components/SsgSwButtons/SsgSwButtons.h"
#include "../NudgeButtons/NudgeButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentSsgSwEnv : public GuiBase {
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
    GuiComponentSsgSwEnv(const GuiContext& context) :
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
        l6Btns(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvSsgSw& copyObj);
    void pasteParams(CopyEnvSsgSw& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
