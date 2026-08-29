#pragma once

#include <JuceHeader.h>
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
        juce::Colour categoryBg = GuiColor::Category::SwBg);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setEnabled(bool enabled);
    void copyParams(CopyEnvSsgHw& copyObj);
    void pasteParams(CopyEnvSsgHw& copyObj);
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();

    // Shape / MIN / MAX / Smooth が変わったら折れ線を作り直す
    // MIN と MAX が互いを押すときの、入れ子呼び出しを弾くための印
    bool isClampingRange = false;

    void updatePreview();
};
