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
#include "../../../Gui/Curve/GuiCurve.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "../../../Gui/Components/SsgSwButtons/SsgSwButtons.h"
#include "../NudgeButtons/NudgeButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../StepValues/StepValues.h"

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
    // 段ごとにつまみを並べる代わりに、対象を選ぶつまみと値のつまみを 1 組ずつ置く。
    // 選んだ段へその場で束縛し直し、選んでいない段の値は帯へまとめて描く。
    // 以前は段の数だけつまみと補正ボタンが並んでいて、この区分ひとつで
    // 200 個近い部品になっていた。
    GuiSlider rateTarget;
    GuiComponentNudgeSliderFloat rate;
    GuiComponentNudgeButtons rateNudge;
    GuiStepValues rateValues;

    NormalSeparator rateSeparator;

    GuiSlider levelTarget;
    GuiComponentNudgeSliderFloat level;
    GuiComponentSsgSwButtons levelBtns;
    GuiStepValues levelValues;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // applyLoopValues の入れ子呼び出しを弾くための印。
    // 中で setValue を呼ぶと onValueChange 経由で自分が呼び返される。
    bool isApplyingLoopValues = false;

    void applyLoopValues(bool enabled);

    // 選んだ段へ値のつまみを束縛し直す。帯の描き直しもここでやる。
    void rebindRate();
    void rebindLevel();

    // 帯に出す値を今の状態から作り直す。
    void refreshStepValues();

    // 段ごとの値。つまみは 1 組しか束縛されていないので、APVTS から直に読み書きする。
    float getStepValue(const juce::String& key) const;
    void setStepValue(const juce::String& key, float value);

    // setup で受け取った接頭辞。束縛し直すときに使う。
    juce::String paramCode;
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
        rateTarget(context),
        rate(context),
        rateNudge(context),
        rateSeparator(context),
        levelTarget(context),
        level(context),
        levelBtns(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx);
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
