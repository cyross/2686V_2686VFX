#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include <array>

#include "../../Processor/Curve/ProcessorCurveValues.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "./GuiCurveGraph.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiCurve : public GuiBase
{
    // Pane Groups
    GuiScrollGroup curveGroup;

    std::array<std::unique_ptr<GuiCurveGraph>, CurvePrValue::params> curveGraphs;

    std::unique_ptr<juce::FileChooser> fileChooser;

    // ヘルパーメソッド
    void updateVisible();
    void updateGraphBinding(int vp);

public:
    GuiCurve(const GuiContext& context);

    GuiComboBox position;
    GuiComboBox target;
    GuiComponentImportExport ieCurve;
    NormalSeparator mainSeparator;

    // 14000個の配列をやめ、現在表示中のタブ(最大16パラメータ)の分だけを保持する
    std::array<std::unique_ptr<GuiLabel>, CurvePrValue::params> paramLabel;
    std::array<std::unique_ptr<GuiComboBox>, CurvePrValue::params> logic;
    std::array<std::unique_ptr<GuiSlider>, CurvePrValue::params> k;

    // Values は [パラメータ数:16] × [最大Value数:16]
    std::array<std::array<std::unique_ptr<GuiSlider>, CurvePrValue::values>, CurvePrValue::params> value;

    // 再帰呼び出し防止フラグ
    bool isUpdatingUI = false;

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void initParams();
    void importCurveParam();

    // 3.0.0 より前の形式を読む
    void setImportingCurveParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeCurveParams(Io::ParamWriter& writer);
    void exportCurveParam();
};
