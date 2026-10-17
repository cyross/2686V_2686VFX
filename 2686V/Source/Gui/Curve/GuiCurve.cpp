#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "../../Core/Editor/EditorGuiText.h"
#include "./GuiCurve.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "Shared/Processor/Curve/ProcessorCurveKeys.h"
#include "Shared/Processor/Curve/ProcessorCurveValues.h"

#include "./GuiCurveValues.h"
#include "./GuiCurveText.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Io/ParamFile.h"
#include "Shared/Core/Gui/GuiRefresh.h"
#include "../../Core/Gui/GuiPluginContext.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat curveFormat{ "curve", 1 };
}

static std::vector<SelectItem> positionItems = {
    {.name = "Common", .value = 1 },
    {.name = "Op1",    .value = 2 },
    {.name = "Op2",    .value = 3 },
    {.name = "Op3",    .value = 4 },
    {.name = "Op4",    .value = 5 },
    {.name = "Op5",    .value = 6 },
    {.name = "Op6",    .value = 7 },
    {.name = "Op7",    .value = 8 },
    {.name = "Op8",    .value = 9 }
};

static std::vector<SelectItem> targetItems = {
    {.name = "RegValue", .value = 1 },
    {.name = "AmpEnv",   .value = 2 },
    {.name = "PitchEnv", .value = 3 },
    {.name = "SsgSwEnv256", .value = 4 },
    {.name = "SsgSwPEnv256", .value = 5 }
};

// ロジックの選択肢。言語で名前が変わるので、その場で作る。
//
// 組み合わせの名前は部品をつないで作る。並びと番号は変えられない
// (保存したファイルとオートメーションが番号を指す)。
static std::vector<SelectItem> curveLogicItems()
{
    auto join = [](std::initializer_list<const I18n::Text*> parts) {
        juce::String name;

        for (const auto* part : parts) {
            if (name.isNotEmpty()) name += " + ";

            name += part->get();
        }

        return name;
    };

    return {
        {.name = join({ &CurveGuiText::Logic::linear }), .value = 1 },
        {.name = join({ &CurveGuiText::Logic::arcExp }), .value = 2 },
        {.name = join({ &CurveGuiText::Logic::arcLog }), .value = 3 },
        {.name = join({ &CurveGuiText::Logic::exp }), .value = 4 },
        {.name = join({ &CurveGuiText::Logic::log }), .value = 5 },
        {.name = join({ &CurveGuiText::Logic::spline1 }), .value = 6 },
        {.name = join({ &CurveGuiText::Logic::spline2 }), .value = 7 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::arcExp }), .value = 8 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::arcLog }), .value = 9 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::exp }), .value = 10 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::log }), .value = 11 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::spline1 }), .value = 12 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::spline2 }), .value = 13 },
        {.name = join({ &CurveGuiText::Logic::arcExp, &CurveGuiText::Logic::linear }), .value = 14 },
        {.name = join({ &CurveGuiText::Logic::arcLog, &CurveGuiText::Logic::linear }), .value = 15 },
        {.name = join({ &CurveGuiText::Logic::exp, &CurveGuiText::Logic::linear }), .value = 16 },
        {.name = join({ &CurveGuiText::Logic::log, &CurveGuiText::Logic::linear }), .value = 17 },
        {.name = join({ &CurveGuiText::Logic::spline1, &CurveGuiText::Logic::linear }), .value = 18 },
        {.name = join({ &CurveGuiText::Logic::spline2, &CurveGuiText::Logic::linear }), .value = 19 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::arcExp, &CurveGuiText::Logic::linear }), .value = 20 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::arcLog, &CurveGuiText::Logic::linear }), .value = 21 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::exp, &CurveGuiText::Logic::linear }), .value = 22 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::log, &CurveGuiText::Logic::linear }), .value = 23 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::spline1, &CurveGuiText::Logic::linear }), .value = 24 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::spline2, &CurveGuiText::Logic::linear }), .value = 25 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::linear }), .value = 26 },
        {.name = join({ &CurveGuiText::Logic::linear, &CurveGuiText::Logic::linear, &CurveGuiText::Logic::linear }), .value = 27 },
        {.name = join({ &CurveGuiText::Logic::spline1, &CurveGuiText::Logic::spline1 }), .value = 28 },
        {.name = join({ &CurveGuiText::Logic::spline2, &CurveGuiText::Logic::spline2 }), .value = 29 },
        {.name = join({ &CurveGuiText::Logic::spline1, &CurveGuiText::Logic::spline1, &CurveGuiText::Logic::spline1 }), .value = 30 },
        {.name = join({ &CurveGuiText::Logic::spline2, &CurveGuiText::Logic::spline2, &CurveGuiText::Logic::spline2 }), .value = 31 },
    };
}

static std::vector< std::vector<juce::String>> paramTitles = {
    {
        "AR",
        "DR(D1R)",
        "SL(D1L)",
        "RR",
        "SR(D2R)",
        "TL"
    },
    {
        "AR",
        "DR(D1R)",
        "RR",
        "SR(D2R)"
    },
    {
        "AR",
        "DR",
        "RR"
    },
    {
        "R1",
        "R2",
        "R3",
        "R4",
        "R5",
        "R6",
        "R7",
        "R8",
        "R9",
        "R10",
        "R11",
        "Loop To"
    },
    {
        "R1",
        "R2",
        "R3",
        "R4",
        "R5",
        "R6",
        "R7",
        "R8",
        "R9",
        "R10",
        "R11",
        "Loop To"
    }
};

static std::vector<int> paramLengthes = { CurvePrValue::regValParams, CurvePrValue::regAmpEnvParams, CurvePrValue::regPitchEnvParams, CurvePrValue::regSsgSwEnv256Params, CurvePrValue::regSsgSwPEnv256Params };

// 読み込むファイルの params の並びを、今の対象のどの番号へ入れるか。
// 3.6.3 までの 6 段の SSG SW ENV のファイル (R1〜R6 / Loop To) は [256] へ入れる。
// 6 段の R1〜R5 は [256] の R1〜R5、R6 (リリース) は R11、Loop To は Loop To へ。
static std::vector<int> importOrder(int t, const juce::File& file)
{
    const auto name = file.getFileName();
    const bool sixStep = t == (int)CurveParams::Target::SsgSwEnv256
        && name.contains("SsgSwEnv") && !name.contains("SsgSwEnv256") && !name.contains("SsgSwEnv11");

    if (sixStep) {
        return { 0, 1, 2, 3, 4,
            (int)CurveParams::TargetSsgSwEnv256::R11, (int)CurveParams::TargetSsgSwEnv256::LoopTo };
    }

    std::vector<int> order;

    for (int vp = 0; vp < paramLengthes[(size_t)t]; vp++) order.push_back(vp);

    return order;
}

// 読み込むときの名前の縛り。[256] の対象は、3.6.3 までの [11] と 6 段のファイルも
// 出すよう、名前の後ろの数字を外して比べる (SsgSwEnv256 → SsgSwEnv)。
static juce::String importNameFilter(const juce::String& ext)
{
    return ext.trimCharactersAtEnd("0123456789");
}

static std::vector<int> valueLengthes = {
    0,    // Linear
    0, 0, // ArcExp ArcLog
    1, 1, // Exp Log
    2, 4, // Sp1 Sp2
    2, 2, // LinearArcExp LinearArcLog
    3, 3, // LinearExp LinearLog
    4, 6, // LinearSp1 LinearSp2
    2, 2, // ArcExpLinear ArcLogLinear
    3, 3, // ExpLinear LogLinear
    4, 6, // Sp1Linear Sp2Linear
    4, 4, // Linear2ArcExp Linear2ArcLog
    5, 5, // Linear2Exp Linear2Log
    6, 8, // Linear2Sp1 Linear2Sp2
    2, 4, // Linear2 Linear3
	6, 10, // Sprine12, Sprine22
    10, 16 // Sprine13, Sprine23
};

static std::vector<std::vector<juce::String>> valueLabels = {
    {}, // Linear
    {}, // ArcExp
    {}, // ArcLog
    {"R"}, // Exp
    {"R"}, // Log
    {"CPX", "CPY"}, // Sp1
    {"CP1X", "CP1Y", "CP2X", "CP2Y"}, // Sp2
    {"VPX", "VPY"}, // LinearArcExp
    {"VPX", "VPY"}, // LinearArcLog
    {"VPX", "VPY", "R"}, // LinearExp
    {"VPX", "VPY", "R"}, // LinearLog
    {"VPX", "VPY", "CPX", "CPY"}, // LinearSp1
    {"VPX", "VPY", "CP1X", "CP1Y", "CP2X", "CP2Y"}, // LinearSp2
    {"VPX", "VPY"}, // ArcExpLinear
    {"VPX", "VPY"}, // ArcLogLinear
    {"R", "VPX", "VPY"}, // ExpLinear
    {"R", "VPX", "VPY"}, // LogLinear
    {"CPX", "CPY", "VPX", "VPY"}, // Sp1Linear
    {"CP1X", "CP1Y", "CP2X", "CP2Y", "VPX", "VPY"}, // Sp2Linear
    {"VP1X", "VP1Y", "VP2X", "VP2Y"}, // Linear2ArcExp
    {"VP1X", "VP1Y", "VP2X", "VP2Y"}, // Linear2ArcLog
    {"VP1X", "VP1Y", "VP2X", "VP2Y", "R"}, // Linear2Exp
    {"VP1X", "VP1Y", "VP2X", "VP2Y", "R"}, // Linear2Log
    {"VP1X", "VP1Y", "VP2X", "VP2Y", "CPX", "CPY"}, // Linear2Sp1
    {"VP1X", "VP1Y", "VP2X", "VP2Y", "CP1X", "CP1Y", "CP2X", "CP2Y"},  // Linear2Sp2
    {"VPX", "VPY"}, // Linear2
    {"VP1X", "VP1Y", "VP2X", "VP2Y"}, // Linear3
    {"VPX", "VPY", "CP1X", "CP1Y", "CP2X", "CP2Y"}, // Sprine12
    {"VPX", "VPY", "CP1X", "CP1Y", "CP2X", "CP2Y", "CP3X", "CP3Y", "CP4X", "CP4Y"}, // Sprine22
    {"VP1X", "VP1Y", "VP2X", "VP2Y", "CP1X", "CP1Y", "CP2X", "CP2Y", "CP3X", "CP3Y"}, // Sprine13
    {"VP1X", "VP1Y", "VP2X", "VP2Y", "CP1X", "CP1Y", "CP2X", "CP2Y", "CP3X", "CP3Y", "CP4X", "CP4Y", "CP5X", "CP5Y", "CP6X", "CP6Y"}, // Sprine23
};

GuiCurve::GuiCurve(const GuiContext& context) :
    GuiBase(context),
    curveGroup(context),
	enable(context),
    position(context),
    target(context),
    ieCurve(context),
    mainSeparator(context)
{
    // グラフとラベルの初期化
    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp] = std::make_unique<GuiLabel>(context);
        curveGraphs[vp] = std::make_unique<GuiCurveGraph>();

        // 多次元配列を廃止し、表示用のコントロールだけを生成
        logic[vp] = std::make_unique<GuiComboBox>(context);
        k[vp] = std::make_unique<GuiSlider>(context);
        for (int vv = 0; vv < CurvePrValue::values; vv++) {
            value[vp][vv] = std::make_unique<GuiSlider>(context);
        }

        for (auto& b : editButtons[vp]) b = std::make_unique<GuiTextButton>(context);
    }

    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
}

void GuiCurve::setup()
{
    // Position が Common で、Target が RegValue のときは Target を AmpEnv に設定
    auto correctTarget = [this]() {
        int p = position.getSelectedItemIndex();
        int t = target.getSelectedItemIndex();

        if (p == 0 && t == 0) {
            target.setSelectedItemIndex(1, juce::sendNotification);
        }
        };

    int tabOrder = 1;

    curveGroup.setup(*this, CurveGuiText::curveGroup);

    enable.setup({ .parent = *this, .id = "", .title = CurveGuiText::enable, .isReset = false });
    enable.setToggleState(pluginOf(ctx).prCurve.getEnable(), juce::dontSendNotification);
    enable.setWantsKeyboardFocus(true);
    enable.setExplicitFocusOrder(++tabOrder);
    enable.onStateChange = [this] {
        if (isUpdatingUI) return;

		pluginOf(ctx).prCurve.setEnable(enable.getToggleState());
		pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);

        updateEnabled();

        editorOf(ctx).resized();
        };

    position.setup({ .parent = *this, .id = "", .title = CurveGuiText::position, .items = positionItems, .isReset = false });
    position.setSelectedItemIndex(0, juce::dontSendNotification);
    position.setWantsKeyboardFocus(true);
    position.setExplicitFocusOrder(++tabOrder);
    position.onChange = [this, correctTarget] {
        if (isUpdatingUI) return;

        correctTarget();
        updateVisible();

        editorOf(ctx).resized(); // レイアウトが変わる可能性があるので再計算
        };

    target.setup({ .parent = *this, .id = "", .title = CurveGuiText::target, .items = targetItems, .isReset = false });
    target.setSelectedItemIndex(0, juce::dontSendNotification);
    target.setWantsKeyboardFocus(true);
    target.setExplicitFocusOrder(++tabOrder);
    target.onChange = [this] {
        if (isUpdatingUI) return;

        updateVisible();

        editorOf(ctx).resized();
        };

    correctTarget();
    
    ieCurve.setupComponent(*this, ++tabOrder, "Curve");
    ieCurve.onClickImport = [this] { importCurveParam(); };
    ieCurve.onClickExport = [this] { exportCurveParam(); };
    
    mainSeparator.setupComponent(*this);

    // 実際に表示するコントロールのセットアップ (16個分だけ)
    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp]->setup({ .parent = curveGroup.contentCanvas, .title = "" });
        paramLabel[vp]->setWantsKeyboardFocus(true);
        paramLabel[vp]->setExplicitFocusOrder(++tabOrder);

        curveGroup.contentCanvas.addAndMakeVisible(curveGraphs[vp].get());

        // CUT / COPY / PASTE / DUPLICATE / CLEAR
        {
            static const std::array<const char*, 5> editNames = { "CUT", "COPY", "PASTE", "DUPLICATE", "CLEAR" };

            for (size_t b = 0; b < editNames.size(); ++b) {
                auto& btn = *editButtons[vp][b];

                btn.setup({ .parent = curveGroup.contentCanvas, .title = editNames[b], .isReset = false });
                btn.setWantsKeyboardFocus(true);
                btn.setExplicitFocusOrder(++tabOrder);
            }

            editButtons[vp][0]->onClick = [this, vp] { copyParam(vp); clearParam(vp); };
            editButtons[vp][1]->onClick = [this, vp] { copyParam(vp); };
            editButtons[vp][2]->onClick = [this, vp] { pasteParam(vp); };
            editButtons[vp][3]->onClick = [this, vp] { duplicateParam(vp); };
            editButtons[vp][4]->onClick = [this, vp] { clearParam(vp); };
        }

        logic[vp]->setup({ .parent = curveGroup.contentCanvas, .id = "", .title = CurveGuiText::logic, .items = curveLogicItems(), .isReset = false });
        logic[vp]->setSelectedItemIndex(0, juce::dontSendNotification);
        logic[vp]->setWantsKeyboardFocus(true);
        logic[vp]->setExplicitFocusOrder(++tabOrder);
        logic[vp]->onChange = [this, vp] {
            if (isUpdatingUI) return; // UI更新中は無視
            int p = position.getSelectedItemIndex();
            int t = target.getSelectedItemIndex();
            int currentLogic = logic[vp]->getSelectedItemIndex();

            pluginOf(ctx).prCurve.setLogic(p, t, vp, currentLogic);
            pluginOf(ctx).bakeCurvesPrim(p, t, vp);

            // ロジックが変わったのでUIを更新
            updateVisible();

            editorOf(ctx).resized();
            };

        k[vp]->setup({ .parent = curveGroup.contentCanvas, .id = "", .title = CurveGuiText::k, .isReset = false });
        k[vp]->setValue(0.0f, juce::dontSendNotification);
        k[vp]->setWantsKeyboardFocus(true);
        k[vp]->setExplicitFocusOrder(++tabOrder);
        k[vp]->onValueChange = [this, vp] {
            if (isUpdatingUI) return;
            int p = position.getSelectedItemIndex();
            int t = target.getSelectedItemIndex();

            pluginOf(ctx).prCurve.setK(p, t, vp, k[vp]->getValue());

            pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);

            curveGraphs[vp]->repaint();
            };

        // ドラッグが終わった瞬間だけ LUT を再計算する
        k[vp]->onDragEnd = [this, vp] {
            int p = position.getSelectedItemIndex();
            int t = target.getSelectedItemIndex();
            pluginOf(ctx).bakeCurvesPrim(p, t, vp);
            };

        for (int vv = 0; vv < CurvePrValue::values; vv++) {
            value[vp][vv]->setup({ .parent = curveGroup.contentCanvas, .id = "", .title = "", .isReset = false });
            value[vp][vv]->setWantsKeyboardFocus(true);
            value[vp][vv]->setExplicitFocusOrder(++tabOrder);

            value[vp][vv]->onValueChange = [this, vp, vv] {
                if (isUpdatingUI) return;
                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();

                pluginOf(ctx).prCurve.setValue(p, t, vp, vv, value[vp][vv]->getValue());

                pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);

                curveGraphs[vp]->repaint();
                };

            // ドラッグが終わった瞬間だけ LUT を再計算する
            value[vp][vv]->onDragEnd = [this, vp] {
                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                pluginOf(ctx).bakeCurvesPrim(p, t, vp);
                };

            curveGraphs[vp]->onGraphDragEnd = [this, vp]() {
                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                pluginOf(ctx).bakeCurvesPrim(p, t, vp);
                };
        }
    }

    updateEnabled();
    updateVisible(); // ここで初回の一括反映を行う
}

void GuiCurve::updateEnabled() {
    bool enabled = pluginOf(ctx).prCurve.getEnable();

    position.setEnabledWithLabel(enabled);
    target.setEnabledWithLabel(enabled);
    ieCurve.setEnable(enabled);

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp]->setEnabled(enabled);
        curveGraphs[vp]->setEnabled(enabled);

        for (auto& b : editButtons[vp]) b->setEnabled(enabled);
    }

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        logic[vp]->setEnabledWithLabel(enabled);

        k[vp]->setEnabledWithLabel(enabled);

        for (int vv = 0; vv < CurvePrValue::values; vv++) {
            value[vp][vv]->setEnabledWithLabel(enabled);
        }
    }
}

// GUIが切り替わったときや初期化時に、プロセッサから値を読み込んでスライダーに反映する
void GuiCurve::updateVisible()
{
    isUpdatingUI = true; // UI更新中のイベント無限ループを防ぐ

    int p = position.getSelectedItemIndex();
    int t = target.getSelectedItemIndex();
    int vpLen = paramLengthes[t];

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        bool isParamVisible = (vp < vpLen);

        int currentLogicNum = pluginOf(ctx).prCurve.getLogic(p, t, vp);

        // Kを表示するかどうかの判定
        bool showK = false;
        switch ((CurveParams::Logic)currentLogicNum) {
        case CurveParams::Logic::Exp:
        case CurveParams::Logic::Log:
        case CurveParams::Logic::LinearExp:
        case CurveParams::Logic::LinearLog:
        case CurveParams::Logic::ExpLinear:
        case CurveParams::Logic::LogLinear:
        case CurveParams::Logic::Linear2Exp:
        case CurveParams::Logic::Linear2Log:
            showK = true;
            break;
        default:
            showK = false;
            break;
        }

        paramLabel[vp]->setVisible(isParamVisible);
        curveGraphs[vp]->setVisible(isParamVisible);

        for (auto& b : editButtons[vp]) b->setVisible(isParamVisible);
        logic[vp]->label.setVisible(isParamVisible);
        logic[vp]->setVisible(isParamVisible);
        k[vp]->label.setVisible(isParamVisible && showK);
        k[vp]->setVisible(isParamVisible && showK);

        if (!isParamVisible) {
            for (int vv = 0; vv < CurvePrValue::values; vv++) {
                value[vp][vv]->label.setVisible(false);
                value[vp][vv]->setVisible(false);
            }
            continue;
        }

        paramLabel[vp]->setText(paramTitles[t][vp], juce::dontSendNotification);

        logic[vp]->setSelectedItemIndex(currentLogicNum, juce::dontSendNotification);
        k[vp]->setValue(pluginOf(ctx).prCurve.getK(p, t, vp), juce::dontSendNotification);

        int valueLen = valueLengthes[currentLogicNum];

        std::vector<GuiSlider*> activeSliders;
        GuiSlider* activeKSlider = k[vp].get();

        for (int vv = 0; vv < CurvePrValue::values; vv++) {
            bool isValueVisible = (vv < valueLen);

            value[vp][vv]->label.setVisible(isValueVisible);
            value[vp][vv]->setVisible(isValueVisible);

            if (isValueVisible) {
                value[vp][vv]->label.setText(valueLabels[currentLogicNum][vv], juce::dontSendNotification);

                float minVal = CurvePrValue::valueMin;
                float maxVal = CurvePrValue::valueMax;
                float step = 0.0001f;

                switch ((CurveParams::Logic)currentLogicNum) {
                case CurveParams::Logic::LinearArcExp: case CurveParams::Logic::LinearArcLog:
                case CurveParams::Logic::LinearExp:    case CurveParams::Logic::LinearLog:
                case CurveParams::Logic::LinearSp1:    case CurveParams::Logic::LinearSp2:
                case CurveParams::Logic::ArcExpLinear: case CurveParams::Logic::ArcLogLinear:
                case CurveParams::Logic::ExpLinear:    case CurveParams::Logic::LogLinear:
                case CurveParams::Logic::Sp1Linear:    case CurveParams::Logic::Sp2Linear:
                    if (vv < 2) { minVal = CurvePrValue::posMin; maxVal = CurvePrValue::posMax; step = 0.01f; }
                    break;
                case CurveParams::Logic::Linear2ArcExp: case CurveParams::Logic::Linear2ArcLog:
                case CurveParams::Logic::Linear2Exp:    case CurveParams::Logic::Linear2Log:
                case CurveParams::Logic::Linear2Sp1:    case CurveParams::Logic::Linear2Sp2:
                case CurveParams::Logic::Linear3:       case CurveParams::Logic::Sprine13:
                case CurveParams::Logic::Sprine23:
                    if (vv < 4) { minVal = CurvePrValue::posMin; maxVal = CurvePrValue::posMax; step = 0.0001f; }
                    break;
                case CurveParams::Logic::Linear2: case CurveParams::Logic::Sprine12: case CurveParams::Logic::Sprine22:
                    if (vv < 2) { minVal = CurvePrValue::posMin; maxVal = CurvePrValue::posMax; step = 0.0001f; }
                    break;
                default:
                    break;
                }

                value[vp][vv]->setRange(minVal, maxVal, step);
                // 値のセット時も絶対にNotificationを送らない
                value[vp][vv]->setValue(pluginOf(ctx).prCurve.getValue(p, t, vp, vv), juce::dontSendNotification);

                activeSliders.push_back(value[vp][vv].get());
            }
        }

        juce::String graphTitle = paramTitles[t][vp];
        curveGraphs[vp]->bindActiveParameters(graphTitle, currentLogicNum, activeSliders, activeKSlider);
    }

    isUpdatingUI = false;
}

// グラフとスライダーのバインディング
void GuiCurve::updateGraphBinding(int vp)
{
    int p = position.getSelectedItemIndex();
    int t = target.getSelectedItemIndex();

    int currentLogicNum = logic[vp]->getSelectedItemIndex();
    int valueLen = valueLengthes[currentLogicNum];

    std::vector<GuiSlider*> activeSliders;
    GuiSlider* activeKSlider = k[vp].get();

    for (int vv = 0; vv < valueLen; vv++) {
        activeSliders.push_back(value[vp][vv].get());
    }

    juce::String graphTitle = paramTitles[t][vp];
    // グラフ側に「参照すべきプロセッサ内のインデックス (p, t, vp)」を直接渡せるようにすると更に良いですが、
    // 現在の bindActiveParameters にスライダーのポインタを渡す仕様でも動作します。
    curveGraphs[vp]->bindActiveParameters(graphTitle, currentLogicNum, activeSliders, activeKSlider);

    // ※スライダーの onValueChange 内で repaint() は呼ばず、
    // GuiCurveGraph クラスの paint メソッドが呼ばれたときに現在のスライダーの値を参照する仕様なので、
    // スライダー変更時（上の onValueChange 内）に editorOf(ctx).repaint() またはグラフの repaint() を呼ぶだけでOKです。
    // （既存のコードの通り）
    auto graphPtr = curveGraphs[vp].get();
    for (auto* sl : activeSliders) {
        // ラムダキャプチャを上書きして再描画処理を追加
        auto originalCallback = sl->onValueChange;
        sl->onValueChange = [originalCallback, graphPtr]() {
            if (originalCallback) originalCallback();
            graphPtr->repaint();
            };
    }
    if (activeKSlider) {
        auto originalCallback = activeKSlider->onValueChange;
        activeKSlider->onValueChange = [originalCallback, graphPtr]() {
            if (originalCallback) originalCallback();
            graphPtr->repaint();
            };
    }
}

void GuiCurve::layout(juce::Rectangle<int> content)
{
    const int scrollHeight = 8000;
    const int graphWidth = 400;
    const int graphHeight = 400;
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);
    int t = target.getSelectedItemIndex();
    int vpLen = paramLengthes[t];
    int px = CurveGuiValue::CurveGroup::Row::Padding::x;
    int py = CurveGuiValue::CurveGroup::Row::Padding::y;
    int lw = CurveGuiValue::CurveGroup::Row::labelWidth;

    auto mainArea = pageArea;
    curveGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(CurveGuiValue::CurveGroup::Padding::width, CurveGuiValue::CurveGroup::Padding::height);
    mmRect.removeFromTop(CurveGuiValue::CurveGroup::Padding::height);

    int titleWidth = CurveGuiValue::CurveGroup::Row::titleWidth;
    int baseWidth = mmRect.getWidth() - titleWidth;
    int mainWidth = baseWidth / 4;
    int valueWidth = (baseWidth - graphWidth) / 2;

    auto enRect = mmRect.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

    enable.setBounds(enRect.removeFromLeft(titleWidth).reduced(px, py));

    auto pRect = enRect.removeFromLeft(mainWidth);
    position.label.setBounds(pRect.removeFromLeft(lw).reduced(px, py));
    position.setBounds(pRect.reduced(px, py));

    auto tRect = enRect.removeFromLeft(mainWidth);
    target.label.setBounds(tRect.removeFromLeft(lw).reduced(px, py));
    target.setBounds(tRect.reduced(px, py));

    auto ieRect = enRect.removeFromLeft(mainWidth);
    ieRect.removeFromLeft(20);
    ieRect.removeFromTop(4);
    ieCurve.layoutComponent(ieRect);

    mainSeparator.layoutComponent(mmRect);

    curveGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    juce::Rectangle<int> mRect(0, 0, curveGroup.getContentWidth(), scrollHeight);

    auto layoutValue = [this, titleWidth, valueWidth, lw, px, py](juce::Rectangle<int>& valueArea, int valNum, int vp, int vv) {
        auto vvRect = valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

        vvRect.removeFromLeft(titleWidth);

        auto vv1Rect = vvRect.removeFromLeft(valueWidth);
        value[vp][vv]->label.setBounds(vv1Rect.removeFromLeft(lw).reduced(px, py));
        value[vp][vv]->setBounds(vv1Rect.reduced(px, py));

        if (vv + 1 == valNum) {
            return;
        }

        auto vv2Rect = vvRect.removeFromLeft(valueWidth);
        value[vp][vv + 1]->label.setBounds(vv2Rect.removeFromLeft(lw).reduced(px, py));
        value[vp][vv + 1]->setBounds(vv2Rect.reduced(px, py));

        if (vv + 2 == valNum) {
            return;
        }

        valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::valuePaddingY);
        };

    // 実際に表示する vpLen 個だけ配置計算を行う
    for (int vp = 0; vp < vpLen; vp++) {
        auto paramArea = mRect.removeFromTop(graphHeight);

        auto valueArea = paramArea.removeFromLeft(paramArea.getWidth() - graphWidth);

        auto vpRect = valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

        auto vpLbRect = vpRect.removeFromLeft(titleWidth);
        paramLabel[vp]->setBounds(vpLbRect.reduced(px, py));

        auto logicRect = vpRect.removeFromLeft(valueWidth);
        logic[vp]->label.setBounds(logicRect.removeFromLeft(lw).reduced(px, py));
        logic[vp]->setBounds(logicRect.reduced(px, py));

        auto kRect = vpRect.removeFromLeft(valueWidth);
        k[vp]->label.setBounds(kRect.removeFromLeft(lw).reduced(px, py));
        k[vp]->setBounds(kRect.reduced(px, py));

        // CUT / COPY / PASTE / DUPLICATE / CLEAR を 1 行に並べる
        {
            auto editRect = valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

            editRect.removeFromLeft(titleWidth);

            const int bw = juce::jmin(96, editRect.getWidth() / 5);

            for (auto& b : editButtons[vp]) b->setBounds(editRect.removeFromLeft(bw).reduced(px, py));
        }

        int logicNum = logic[vp]->getSelectedItemIndex();
        int valuesCount = valueLengthes[logicNum];

        valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::valueSpacingY);

        for (int vv = 0; vv < valuesCount; vv += 2) {
            layoutValue(valueArea, valuesCount, vp, vv);
        }

        auto graphArea = paramArea.reduced(20, 20);
        curveGraphs[vp]->setBounds(graphArea);

        if (vp < vpLen - 1) {
            mRect.removeFromTop(CurveGuiValue::CurveGroup::Row::valuePaddingY);
        }
    }

    int usedHeight = scrollHeight - mRect.getHeight();
    curveGroup.setContentHeight(usedHeight);
}

void GuiCurve::initParams()
{
    pluginOf(ctx).prCurve.resetToDefault();
    pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);
    updateVisible();
}

void GuiCurve::importCurveParam() {
    juce::File defaultDir(pluginOf(ctx).defaultCurveParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = pluginOf(ctx).getPluginDirectory();
    }

    int posIndex = position.getSelectedItemIndex();

    juce::String posExt = posIndex == 0 ? "common" : "op";

    int targetIndex = target.getSelectedItemIndex();

    juce::String targetExt = targetItems[targetIndex].name;

    juce::String ext = posExt + "_" + targetExt;

    // 一覧から選ぶ。CURVE は置き場所と対象で中身が違うので、
    // 区分だけでなく名前でも縛る。
    editorOf(ctx).openParamBrowser(pluginOf(ctx).defaultCurveParamDir,
        { EditorGuiText::ParamBrowser::kindCurve },
        [this](const juce::File& file) {
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                pluginOf(ctx).defaultCurveParamDir = file.getParentDirectory().getFullPathName();

                // 3.0.0 より前のファイルは、当時の処理で読み込んでから
                // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
                // 読み込みは当時のものをそのまま使う。
                if (Io::isLegacyFile(file)) {
                    juce::StringArray lines;

                    file.readLines(lines);

                    int index = 0;

                    {
                        // 読み終えてからまとめて描き直す
                        GuiRefresh::Batch batch;

                        setImportingCurveParams(lines, index, importOrder(target.getSelectedItemIndex(), file));
                    }

                    Io::ParamWriter writer(curveFormat);

                    writeCurveParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, curveFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                const auto order = importOrder(t, file);

                // 今出している Position と Target のぶんだけ戻す。
                // パラメータごとに入れ子で持つので、数が違っても後ろがずれない。
                for (int i = 0; i < (int)order.size(); i++) {
                    const int vp = order[(size_t)i];
                    auto item = reader->arrayItem("params", i);

                    pluginOf(ctx).prCurve.setLogic(p, t, vp,
                        item.getInt("logic", pluginOf(ctx).prCurve.getLogic(p, t, vp)));
                    pluginOf(ctx).prCurve.setK(p, t, vp,
                        item.getFloat("k", pluginOf(ctx).prCurve.getK(p, t, vp)));

                    auto values = item.getFloatArray("values");

                    for (int vv = 0; vv < CurvePrValue::values && vv < (int)values.size(); vv++) {
                        pluginOf(ctx).prCurve.setValue(p, t, vp, vv, values[(size_t)vv]);
                    }
                }

                // プロセッサ側でカーブ計算を再実行し、コアに反映
                pluginOf(ctx).bakeCurves();
                pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);

                // GUIのコンポーネント（スライダーや表示状態）を最新値に更新
                updateVisible();
                editorOf(ctx).resized();
            }
        },
        importNameFilter(Io::Extension::curveParam + ext));
}

void GuiCurve::exportCurveParam() {
    juce::File defaultDir(pluginOf(ctx).defaultCurveParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = pluginOf(ctx).getPluginDirectory();
    }

    int posIndex = position.getSelectedItemIndex();

    juce::String posExt = posIndex == 0 ? "common" : "op";

    int targetIndex = target.getSelectedItemIndex();

    juce::String targetExt = targetItems[targetIndex].name;

    juce::String ext = posExt + "_" + targetExt;

    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    editorOf(ctx).openParamBrowserToSave(pluginOf(ctx).defaultCurveParamDir,
        { EditorGuiText::ParamBrowser::kindCurve }, Io::Extension::curveParam + ext,
        [this](const juce::File& file) {
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                pluginOf(ctx).defaultCurveParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(curveFormat);
                writeCurveParams(writer);

                writer.writeTo(file);
            }
        },
        Io::Extension::curveParam + ext);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiCurve::setImportingCurveParams(juce::StringArray& lines, int& index, const std::vector<int>& order) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	int p = position.getSelectedItemIndex();
	int t = target.getSelectedItemIndex();

	// 現在表示中の Position と Target に該当するパラメータ群のみ復元
	for (const int vp : order) {
	    if (index < size) {
	        int logicVal = lines[index++].getIntValue();
	        pluginOf(ctx).prCurve.setLogic(p, t, vp, logicVal);
	    }
	    if (index < size) {
	        float kVal = lines[index++].getFloatValue();
	        pluginOf(ctx).prCurve.setK(p, t, vp, kVal);
	    }

	    // Value配列は使われていない分も含め、常に最大要素数(16)分を安全に読み込む
	    for (int vv = 0; vv < CurvePrValue::values; vv++) {
	        if (index < size) {
	            float val = lines[index++].getFloatValue();
	            pluginOf(ctx).prCurve.setValue(p, t, vp, vv, val);
	        }
	    }
	}

	// プロセッサ側でカーブ計算を再実行し、コアに反映
	pluginOf(ctx).bakeCurves();
	pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);

	// GUIのコンポーネント（スライダーや表示状態）を最新値に更新
	updateVisible();
	editorOf(ctx).resized();

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiCurve::writeCurveParams(Io::ParamWriter& writer) {
	int p = position.getSelectedItemIndex();
	int t = target.getSelectedItemIndex();
	int vpLen = paramLengthes[t];

	// 今出している Position と Target のぶんだけ書く
	for (int vp = 0; vp < vpLen; vp++) {
	    auto item = writer.arrayItem("params", vp);

	    item.set("logic", pluginOf(ctx).prCurve.getLogic(p, t, vp));
	    item.set("k", pluginOf(ctx).prCurve.getK(p, t, vp));

	    std::vector<float> values;

	    for (int vv = 0; vv < CurvePrValue::values; vv++) {
	        values.push_back(pluginOf(ctx).prCurve.getValue(p, t, vp, vv));
	    }

	    item.setArray("values", values);
	}


}

// ============================================================================
// パラメータごとの CUT / COPY / PASTE / DUPLICATE / CLEAR (4.0.0)
// ============================================================================
namespace
{
    // 写したもの。Position・Target を問わず貼れるよう、1 つだけ持つ
    struct CurveClip
    {
        bool valid = false;
        int logic = 0;
        float k = 1.0f;
        std::array<float, CurvePrValue::values> values{};
    };

    CurveClip& curveClip()
    {
        static CurveClip clip;
        return clip;
    }
}

// 書き換えたカーブを処理側へ渡し、画面を読み直す
void GuiCurve::applyCurveChanges()
{
    pluginOf(ctx).bakeCurves();
    pluginOf(ctx).getCurveCore()->setParameters(pluginOf(ctx).prCurve.m_curveParams);

    updateVisible();
    editorOf(ctx).resized();
}

void GuiCurve::copyParam(int vp)
{
    const int p = position.getSelectedItemIndex();
    const int t = target.getSelectedItemIndex();
    auto& pr = pluginOf(ctx).prCurve;
    auto& clip = curveClip();

    clip.valid = true;
    clip.logic = pr.getLogic(p, t, vp);
    clip.k = pr.getK(p, t, vp);

    for (int vv = 0; vv < CurvePrValue::values; ++vv) clip.values[(size_t)vv] = pr.getValue(p, t, vp, vv);
}

void GuiCurve::pasteParam(int vp)
{
    const auto& clip = curveClip();

    if (!clip.valid) return;

    const int p = position.getSelectedItemIndex();
    const int t = target.getSelectedItemIndex();
    auto& pr = pluginOf(ctx).prCurve;

    pr.setLogic(p, t, vp, clip.logic);
    pr.setK(p, t, vp, clip.k);

    for (int vv = 0; vv < CurvePrValue::values; ++vv) pr.setValue(p, t, vp, vv, clip.values[(size_t)vv]);

    applyCurveChanges();
}

void GuiCurve::clearParam(int vp)
{
    const int p = position.getSelectedItemIndex();
    const int t = target.getSelectedItemIndex();
    auto& pr = pluginOf(ctx).prCurve;

    pr.setLogic(p, t, vp, CurvePrValue::Logic::initial);
    pr.setK(p, t, vp, CurvePrValue::K::initial);

    for (int vv = 0; vv < CurvePrValue::values; ++vv) pr.setValue(p, t, vp, vv, CurvePrValue::Value::initial);

    applyCurveChanges();
}

// 同じ Position・Target のほかのパラメータへ写す。写し先はダイアログで選ぶ
void GuiCurve::duplicateParam(int vp)
{
    const int t = target.getSelectedItemIndex();
    const int vpLen = paramLengthes[(size_t)t];

    if (vpLen < 2) return;

    if (duplicateDialog == nullptr) duplicateDialog = std::make_unique<GuiTargetsDialog>(ctx);

    GuiTargetsDialog::Config c;

    c.mode = GuiTargetsDialog::Mode::DuplicateTo;
    c.count = vpLen;
    c.current = vp;
    c.title = "DUPLICATE: " + paramTitles[(size_t)t][(size_t)vp];

    for (int i = 0; i < vpLen; ++i) c.names.push_back(paramTitles[(size_t)t][(size_t)i]);

    c.onDuplicateTo = [this, vp](const std::vector<int>& targets) {
        const int p = position.getSelectedItemIndex();
        const int t = target.getSelectedItemIndex();
        auto& pr = pluginOf(ctx).prCurve;

        for (int to : targets) {
            if (to == vp) continue;

            pr.setLogic(p, t, to, pr.getLogic(p, t, vp));
            pr.setK(p, t, to, pr.getK(p, t, vp));

            for (int vv = 0; vv < CurvePrValue::values; ++vv) pr.setValue(p, t, to, vv, pr.getValue(p, t, vp, vv));
        }

        applyCurveChanges();
    };

    constexpr int width = 380;

    duplicateDialog->open(*this, getLocalBounds().withSizeKeepingCentre(juce::jmin(width, getWidth() - 20), juce::jmax(100, getHeight() - 40)), c);
}
