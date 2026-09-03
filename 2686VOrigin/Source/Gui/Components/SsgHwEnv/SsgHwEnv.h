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
#include "../../../Gui/Components/WavePreview/WavePreview.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "../../../Gui/Components/SsgSwButtons/SsgSwButtons.h"

#include "../../../Core/Gui/GuiCopyObj.h"

class GuiComponentSsgHwEnv : public GuiBase {
    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    // SSG SW ENV
    GuiCategoryLabel cat;
    GuiToggleButton envEnableButton;
    GuiToggleButton smoothEnableButton;
    NormalSeparator hwEnvSeparator;
    GuiComboBox shapeSelector;
    GuiSlider periodSlider;
    GuiSlider minSlider;
    GuiSlider maxSlider;

    // 選んだ Shape がどんな形かを見せる
    GuiWavePreview preview;

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
    GuiComponentSsgHwEnv(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        envEnableButton(context),
        smoothEnableButton(context),
        hwEnvSeparator(context),
        shapeSelector(context),
        periodSlider(context),
        minSlider(context),
        maxSlider(context),
        preview(context)
    {
    }

    // categoryBg は見出しの背景色。SSG チャンネル自身の機能なので、
    // SSG では HwBg、他チャンネルへ借りて置く場合は既定の SwBg を使う。
    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder,
        juce::Colour categoryBg = GuiColor::Category::SwAmpBg);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvSsgHw& copyObj);
    void pasteParams(CopyEnvSsgHw& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();

    // Shape / MIN / MAX / Smooth が変わったら折れ線を作り直す
    // MIN と MAX が互いを押すときの、入れ子呼び出しを弾くための印
    bool isClampingRange = false;

    void updatePreview();
};
