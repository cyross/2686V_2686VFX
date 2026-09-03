#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>
#include <vector>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "../NudgeSlider/NudgeSliderFloat.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentFix : public GuiBase {
    static std::array<juce::String, 128> noteLabelText;

    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiToggleButton enable;
    GuiComponentNudgeSliderFloat freq;
    NormalSeparator freqToSeparator;
    GuiTextButton freqTo;
    GuiTextButton freqToZero;
    GuiTextButton freqTo05;
    GuiTextButton freqTo1;
    GuiTextButton freqTo2;
    GuiTextButton freqM05;
    GuiTextButton freqM02;
    GuiTextButton freqM01;
    GuiTextButton freqP01;
    GuiTextButton freqP02;
    GuiTextButton freqP05;
    GuiTextButton freqM5;
    GuiTextButton freqM2;
    GuiTextButton freqM1;
    GuiTextButton freqP1;
    GuiTextButton freqP2;
    GuiTextButton freqP5;
    GuiTextButton freqM50;
    GuiTextButton freqM20;
    GuiTextButton freqM10;
    GuiTextButton freqP10;
    GuiTextButton freqP20;
    GuiTextButton freqP50;
    GuiTextButton freqM500;
    GuiTextButton freqM200;
    GuiTextButton freqM100;
    GuiTextButton freqP100;
    GuiTextButton freqP200;
    GuiTextButton freqP500;
    GuiTextButton freqM1000;
    GuiTextButton freqP1000;
    NormalSeparator freqNoteSeparator;
    GuiSlider freqNote;
    GuiLabel note;
    GuiTextButton applyNote;
    GuiTextButton applyToC3;
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

    bool isCategoryBypassed() const { return !enable.getToggleState(); }

    void setCategoryBypassed(bool bypassed) {
        enable.setToggleState(!bypassed, juce::sendNotification);
    }

    // 見出しの開閉
    void setCategoryOpen(bool open) { cat.setDetailVisible(open); }
    GuiComponentFix(const GuiContext& context) :
        GuiBase(context),
        cat(context),
		enable(context),
		freq(context),
        freqToSeparator(context),
		freqTo(context),
        freqToZero(context),
        freqTo05(context),
        freqTo1(context),
        freqTo2(context),
        freqM05(context),
        freqM02(context),
        freqM01(context),
        freqP01(context),
        freqP02(context),
        freqP05(context),
        freqM5(context),
        freqM2(context),
        freqM1(context),
        freqP1(context),
        freqP2(context),
        freqP5(context),
        freqM50(context),
        freqM20(context),
        freqM10(context),
        freqP10(context),
        freqP20(context),
        freqP50(context),
        freqM500(context),
        freqM200(context),
        freqM100(context),
        freqP100(context),
        freqP200(context),
        freqP500(context),
        freqM1000(context),
        freqP1000(context),
        freqNoteSeparator(context),
        freqNote(context),
        note(context),
        applyNote(context),
        applyToC3(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& toLabel, int toValue, bool isHw = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnables(bool enabled);
    void copyParams(CopyFix& copyObj);
    void pasteParams(CopyFix& copyObj);
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
