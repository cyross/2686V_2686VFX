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
#include "../../../Gui/Curve/GuiCurve.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"
#include "../PitchButtons/PitchButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../Separator/NormalSeparator.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentUnison : public GuiBase {
    // UNISON/HARMONY
    GuiCategoryLabel cat;
    GuiSlider voices;
    GuiComponentNudgeSliderFloat detune;
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
    GuiComponentNudgeSliderFloat paraDetune;
    GuiComponentPitchButtons paraDetuneButtons;
    juce::String paramCode;

    void rebindParaSliders();
    float getParaValue(const juce::String& key, int voiceIndex);
    void setParaValue(const juce::String& key, int voiceIndex, float value);

    std::unique_ptr<juce::FileChooser> fileChooser;

public:

    // 簡易表示モードで丸ごと隠す。見出しごと消え、縦の場所も取らない。
    //
    // 見出しを見せるかどうかはレイアウト側では戻らない (あちらは場所を
    // 決めるだけ) ので、ここで両方向とも面倒を見る。
    void setCategoryVisible(bool visible) {
        cat.setHidden(!visible);
        cat.setVisible(visible);
    }
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
    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
