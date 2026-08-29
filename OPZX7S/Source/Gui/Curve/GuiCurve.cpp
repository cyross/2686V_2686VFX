#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiCurve.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Curve/ProcessorCurveKeys.h"
#include "../../Processor/Curve/ProcessorCurveValues.h"

#include "./GuiCurveValues.h"
#include "./GuiCurveText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> positionItems = {
    {.name = "Common",    .value = 1 },
    {.name = "Op1",       .value = 2 },
    {.name = "Op2",       .value = 3 },
    {.name = "Op3",       .value = 4 },
    {.name = "Op4",       .value = 5 },
    {.name = "Op5",       .value = 6 },
    {.name = "Op6",       .value = 7 },
    {.name = "Op7",       .value = 8 },
    {.name = "Op8",       .value = 9 }
};

static std::vector<SelectItem> targetItems = {
    {.name = "RegValue", .value = 1 },
    {.name = "AmpEnv",   .value = 2 },
    {.name = "PitchEnv", .value = 3 },
    {.name = "SsgSwEnv", .value = 4 },
    {.name = "SsgSwEnv11", .value = 5 },
    {.name = "SsgSwPEnv11", .value = 6 }
};

static std::vector<SelectItem> logicItems = {
    {.name = juce::String("") + "線形",                                          .value = 1 },
    {.name = juce::String("") + "円弧(指数的)",                                  .value = 2 },
    {.name = juce::String("") + "円弧(対数的)",                                  .value = 3 },
    {.name = juce::String("") + "指数関数",                                      .value = 4 },
    {.name = juce::String("") + "対数関数",                                      .value = 5 },
    {.name = juce::String("") + "1点スプライン",                                 .value = 6 },
    {.name = juce::String("") + "2点スプライン",                                 .value = 7 },
    {.name = juce::String("") + "線形 + 円弧(指数的)",                           .value = 8 },
    {.name = juce::String("") + "線形 + 円弧(対数的)",                           .value = 9 },
    {.name = juce::String("") + "線形 + 指数関数",                               .value = 10 },
    {.name = juce::String("") + "線形 + 対数関数",                               .value = 11 },
    {.name = juce::String("") + "線形 + 1点スプライン",                          .value = 12 },
    {.name = juce::String("") + "線形 + 2点スプライン",                          .value = 13 },
    {.name = juce::String("") + "円弧(指数的) + 線形",                           .value = 14 },
    {.name = juce::String("") + "円弧(対数的) + 線形",                           .value = 15 },
    {.name = juce::String("") + "指数関数 + 線形",                               .value = 16 },
    {.name = juce::String("") + "対数関数 + 線形",                               .value = 17 },
    {.name = juce::String("") + "1点スプライン + 線形",                          .value = 18 },
    {.name = juce::String("") + "2点スプライン + 線形",                          .value = 19 },
    {.name = juce::String("") + "線形 + 円弧(指数的) + 線形",                    .value = 20 },
    {.name = juce::String("") + "線形 + 円弧(対数的) + 線形",                    .value = 21 },
    {.name = juce::String("") + "線形 + 指数関数 + 線形",                        .value = 22 },
    {.name = juce::String("") + "線形 + 対数関数 + 線形",                        .value = 23 },
    {.name = juce::String("") + "線形 + 1点スプライン + 線形",                   .value = 24 },
    {.name = juce::String("") + "線形 + 2点スプライン + 線形",                   .value = 25 },
    {.name = juce::String("") + "線形 + 線形",                                   .value = 26 },
    {.name = juce::String("") + "線形 + 線形 + 線形",                            .value = 27 },
    {.name = juce::String("") + "1点スプライン + 1点スプライン",                 .value = 28 },
    {.name = juce::String("") + "2点スプライン + 2点スプライン",                 .value = 29 },
    {.name = juce::String("") + "1点スプライン + 1点スプライン + 1点スプライン", .value = 30 },
    {.name = juce::String("") + "2点スプライン + 2点スプライン + 2点スプライン", .value = 31 },
};

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

static std::vector<int> paramLengthes = { CurvePrValue::regValParams, CurvePrValue::regAmpEnvParams, CurvePrValue::regPitchEnvParams, CurvePrValue::regSsgSwEnvParams, CurvePrValue::regSsgSwEnv11Params, CurvePrValue::regSsgSwPEnv11Params };

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

    position.setup({ .parent = *this, .id = "", .title = CurveGuiText::position, .items = positionItems, .isReset = false });
    position.setSelectedItemIndex(0, juce::dontSendNotification);
    position.setWantsKeyboardFocus(true);
    position.setExplicitFocusOrder(++tabOrder);
    position.onChange = [this, correctTarget] {
        if (isUpdatingUI) return;
        correctTarget();
        updateVisible();
        ctx.editor.resized(); // レイアウトが変わる可能性があるので再計算
        };

    target.setup({ .parent = *this, .id = "", .title = CurveGuiText::target, .items = targetItems, .isReset = false });
    target.setSelectedItemIndex(0, juce::dontSendNotification);
    target.setWantsKeyboardFocus(true);
    target.setExplicitFocusOrder(++tabOrder);
    target.onChange = [this] {
        if (isUpdatingUI) return;
        updateVisible();
        ctx.editor.resized();
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

        logic[vp]->setup({ .parent = curveGroup.contentCanvas, .id = "", .title = CurveGuiText::logic, .items = logicItems, .isReset = false });
		logic[vp]->setSelectedItemIndex(0, juce::dontSendNotification);
        logic[vp]->setWantsKeyboardFocus(true);
        logic[vp]->setExplicitFocusOrder(++tabOrder);
        logic[vp]->onChange = [this, vp] {
            if (isUpdatingUI) return; // UI更新中は無視
            int p = position.getSelectedItemIndex();
            int t = target.getSelectedItemIndex();
            int currentLogic = logic[vp]->getSelectedItemIndex();

            ctx.audioProcessor.prCurve.setLogic(p, t, vp, currentLogic);
            ctx.audioProcessor.bakeCurvesPrim(p, t, vp);

            // ロジックが変わったのでUIを更新
            updateVisible();
            ctx.editor.resized();
            };

        k[vp]->setup({ .parent = curveGroup.contentCanvas, .id = "", .title = CurveGuiText::k, .isReset = false });
        k[vp]->setValue(0.0f, juce::dontSendNotification);
        k[vp]->setWantsKeyboardFocus(true);
        k[vp]->setExplicitFocusOrder(++tabOrder);
        k[vp]->onValueChange = [this, vp] {
            if (isUpdatingUI) return;
            int p = position.getSelectedItemIndex();
            int t = target.getSelectedItemIndex();

            ctx.audioProcessor.prCurve.setK(p, t, vp, k[vp]->getValue());

            ctx.audioProcessor.getCurveCore()->setParameters(ctx.audioProcessor.prCurve.m_curveParams);

            curveGraphs[vp]->repaint();
            };

        // ドラッグが終わった瞬間だけ LUT を再計算する
        k[vp]->onDragEnd = [this, vp] {
            int p = position.getSelectedItemIndex();
            int t = target.getSelectedItemIndex();
            ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
            };

        for (int vv = 0; vv < CurvePrValue::values; vv++) {
            value[vp][vv]->setup({ .parent = curveGroup.contentCanvas, .id = "", .title = "", .isReset = false });
            value[vp][vv]->setWantsKeyboardFocus(true);
            value[vp][vv]->setExplicitFocusOrder(++tabOrder);

            value[vp][vv]->onValueChange = [this, vp, vv] {
                if (isUpdatingUI) return;
                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();

                ctx.audioProcessor.prCurve.setValue(p, t, vp, vv, value[vp][vv]->getValue());

                ctx.audioProcessor.getCurveCore()->setParameters(ctx.audioProcessor.prCurve.m_curveParams);

                curveGraphs[vp]->repaint();
                };

            // ドラッグが終わった瞬間だけ LUT を再計算する
            value[vp][vv]->onDragEnd = [this, vp] {
                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
                };

            curveGraphs[vp]->onGraphDragEnd = [this, vp]() {
                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
                };
        }
    }

    updateVisible(); // ここで初回の一括反映を行う
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

        int currentLogicNum = ctx.audioProcessor.prCurve.getLogic(p, t, vp);

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
        k[vp]->setValue(ctx.audioProcessor.prCurve.getK(p, t, vp), juce::dontSendNotification);

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
                value[vp][vv]->setValue(ctx.audioProcessor.prCurve.getValue(p, t, vp, vv), juce::dontSendNotification);

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
    // スライダー変更時（上の onValueChange 内）に ctx.editor.repaint() またはグラフの repaint() を呼ぶだけでOKです。
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
    ctx.audioProcessor.prCurve.resetToDefault(); 
    updateVisible();
}

void GuiCurve::importCurveParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultCurveParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    int posIndex = position.getSelectedItemIndex();

    juce::String posExt = posIndex == 0 ? "common" : "op";

    int targetIndex = target.getSelectedItemIndex();

    juce::String targetExt = targetItems[targetIndex].name;

    juce::String ext = posExt + "_" + targetExt;

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importCurveParamFile, defaultDir, Io::ExtensionGlob::curveParam + ext);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultCurveParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                int vpLen = paramLengthes[t];

                // 現在表示中の Position と Target に該当するパラメータ群のみ復元
                for (int vp = 0; vp < vpLen; vp++) {
                    if (index < size) {
                        int logicVal = lines[index++].getIntValue();
                        ctx.audioProcessor.prCurve.setLogic(p, t, vp, logicVal);
                    }
                    if (index < size) {
                        float kVal = lines[index++].getFloatValue();
                        ctx.audioProcessor.prCurve.setK(p, t, vp, kVal);
                    }

                    // Value配列は使われていない分も含め、常に最大要素数(16)分を安全に読み込む
                    for (int vv = 0; vv < CurvePrValue::values; vv++) {
                        if (index < size) {
                            float val = lines[index++].getFloatValue();
                            ctx.audioProcessor.prCurve.setValue(p, t, vp, vv, val);
                        }
                    }
                }

                // プロセッサ側でカーブ計算を再実行し、コアに反映
                ctx.audioProcessor.bakeCurves();
                ctx.audioProcessor.getCurveCore()->setParameters(ctx.audioProcessor.prCurve.m_curveParams);

                // GUIのコンポーネント（スライダーや表示状態）を最新値に更新
                updateVisible();
                ctx.editor.resized();
            }
        });
}

void GuiCurve::exportCurveParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultCurveParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    int posIndex = position.getSelectedItemIndex();

    juce::String posExt = posIndex == 0 ? "common" : "op";

    int targetIndex = target.getSelectedItemIndex();

    juce::String targetExt = targetItems[targetIndex].name;

    juce::String ext = posExt + "_" + targetExt;

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default." + Io::Extension::curveParam + ext), Io::ExtensionGlob::curveParam + ext);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultCurveParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                int p = position.getSelectedItemIndex();
                int t = target.getSelectedItemIndex();
                int vpLen = paramLengthes[t];

                // 現在表示中の Position と Target に該当するパラメータ群のみ保存
                for (int vp = 0; vp < vpLen; vp++) {
                    content += juce::String(ctx.audioProcessor.prCurve.getLogic(p, t, vp)) + "\n";
                    content += juce::String(ctx.audioProcessor.prCurve.getK(p, t, vp), Global::floatDecimalPlaces) + "\n";

                    for (int vv = 0; vv < CurvePrValue::values; vv++) {
                        content += juce::String(ctx.audioProcessor.prCurve.getValue(p, t, vp, vv), Global::floatDecimalPlaces) + "\n";
                    }
                }

                file.replaceWithText(content);
            }
        });
}
