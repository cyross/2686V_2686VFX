#include <vector>

#include "./GuiCurve.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Curve/ProcessorCurveKeys.h"
#include "../../Processor/Curve/ProcessorCurveValues.h"

#include "./GuiCurveValues.h"
#include "./GuiCurveText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> positionItems = {
    {.name = "Common", .value = 1 },
    {.name = "Op1",    .value = 2 },
    {.name = "Op2",    .value = 3 },
    {.name = "Op3",    .value = 4 },
    {.name = "Op4",    .value = 5 },
    {.name = "Op5",    .value = 6 },
    {.name = "Op6",    .value = 7 }
};

static std::vector<SelectItem> targetItems = {
    {.name = "RegValue", .value = 1 },
    {.name = "AmpEnv",   .value = 2 },
    {.name = "PitchEnv", .value = 3 },
    {.name = "SsgSwEnv", .value = 4 }
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
    }
};

static std::vector<int> paramLengthes = { CurvePrValue::regValParams, CurvePrValue::regAmpEnvParams, CurvePrValue::regPitchEnvParams, CurvePrValue::regSsgSwEnvParams };

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
    mainSeparator(context)
{
    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp] = std::make_unique<GuiLabel>(context);
        curveGraphs[vp] = std::make_unique<GuiCurveGraph>();
    }

    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                logic[p][t][vp] = std::make_unique<GuiComboBox>(context);
                k[p][t][vp] = std::make_unique<GuiSlider>(context);
                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    value[p][t][vp][vv] = std::make_unique<GuiSlider>(context);
                }
            }
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
            target.setSelectedItemIndex(1);
        }
        };

    auto setRangeByLogic = [this](int currentLogic, int p, int t, int vp, int vv) {
        // 1. 対象となるパラメータのIDを正確に再構築する
        juce::String paramID = CurvePrKey::prefix
            + CurvePrKey::position[p]
            + CurvePrKey::target[t]
            + CurvePrKey::paramList[vp]
            + CurvePrKey::valueList[vv];

        float minVal = CurvePrValue::valueMin;  // -5.0f
        float maxVal = CurvePrValue::valueMax;  //  5.0f
        float step = 0.0001f;

        // 2. ロジックに応じて範囲を決定
        switch ((CurveParams::Logic)currentLogic) {
        case CurveParams::Logic::LinearArcExp:
        case CurveParams::Logic::LinearArcLog:
        case CurveParams::Logic::LinearExp:
        case CurveParams::Logic::LinearLog:
        case CurveParams::Logic::LinearSp1:
        case CurveParams::Logic::LinearSp2:
        case CurveParams::Logic::ArcExpLinear:
        case CurveParams::Logic::ArcLogLinear:
        case CurveParams::Logic::ExpLinear:
        case CurveParams::Logic::LogLinear:
        case CurveParams::Logic::Sp1Linear:
        case CurveParams::Logic::Sp2Linear:
            if (vv >= 0 && vv < 2) {
                minVal = CurvePrValue::posMin;  // 0.0f
                maxVal = CurvePrValue::posMax;  // 1.0f
                step = 0.01f;
            }
            break;
        case CurveParams::Logic::Linear2ArcExp:
        case CurveParams::Logic::Linear2ArcLog:
        case CurveParams::Logic::Linear2Exp:
        case CurveParams::Logic::Linear2Log:
        case CurveParams::Logic::Linear2Sp1:
        case CurveParams::Logic::Linear2Sp2:
            if (vv >= 0 && vv < 4) {
                minVal = CurvePrValue::posMin;
                maxVal = CurvePrValue::posMax;
                step = 0.0001f;
            }
            break;
        case CurveParams::Logic::Linear2:
            if (vv >= 0 && vv < 2) {
                minVal = CurvePrValue::posMin;
                maxVal = CurvePrValue::posMax;
                step = 0.0001f;
            }
            break;
        case CurveParams::Logic::Linear3:
            if (vv >= 0 && vv < 4) {
                minVal = CurvePrValue::posMin;
                maxVal = CurvePrValue::posMax;
                step = 0.0001f;
            }
            break;
        case CurveParams::Logic::Sprine12:
        case CurveParams::Logic::Sprine22:
            if (vv >= 0 && vv < 2) {
                minVal = CurvePrValue::posMin;
                maxVal = CurvePrValue::posMax;
                step = 0.0001f;
            }
            break;
        case CurveParams::Logic::Sprine13:
        case CurveParams::Logic::Sprine23:
            if (vv >= 0 && vv < 4) {
                minVal = CurvePrValue::posMin;
                maxVal = CurvePrValue::posMax;
                step = 0.0001f;
            }
            break;
        default:
            break;
        }

        // APVTSから生のパラメータを取り出し、不変と思われがちな range を直接上書きする
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(ctx.apvts.getParameter(paramID)))
        {
            param->range = juce::NormalisableRange<float>(minVal, maxVal, step);
        }

        // スライダー自身の範囲も更新する
        value[p][t][vp][vv]->setRange(minVal, maxVal, step);

        // 値を初期化（すでに記述されている処理）
        value[p][t][vp][vv]->setValue(0.0f, juce::sendNotification);
        };

    // Logic によって x, y のスライダー制限を変更する
    auto correctPos = [this, setRangeByLogic]() {
        auto correctPosition = [this](int p, int t, int vp, int vv) {
            float pos1y = value[p][t][vp][vv]->getValue();

            if (pos1y < CurvePrValue::posMin) {
                value[p][t][vp][vv]->setValue(CurvePrValue::posMin, juce::sendNotification);
            }
            else if (pos1y > CurvePrValue::posMax) {
                value[p][t][vp][vv]->setValue(CurvePrValue::posMax, juce::sendNotification);
            }

            ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
            };

        auto correctPosition0 = [this](int p, int t, int vp) {
            float pos1x = value[p][t][vp][0]->getValue();
            float pos2x = value[p][t][vp][2]->getValue();

            if (pos1x < CurvePrValue::posMin) {
                value[p][t][vp][0]->setValue(CurvePrValue::posMin, juce::sendNotification);
            }
            else if (pos1x > CurvePrValue::posMax) {
                value[p][t][vp][0]->setValue(CurvePrValue::posMax, juce::sendNotification);
            }
            else if (pos1x > pos2x) {
                value[p][t][vp][0]->setValue(pos1x, juce::sendNotification);
            }

            ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
            };

        auto correctPosition2 = [this](int p, int t, int vp) {
            float pos1x = value[p][t][vp][0]->getValue();
            float pos2x = value[p][t][vp][2]->getValue();

            if (pos2x < CurvePrValue::posMin) {
                value[p][t][vp][2]->setValue(CurvePrValue::posMin, juce::sendNotification);
            }
            else if (pos2x > CurvePrValue::posMax) {
                value[p][t][vp][2]->setValue(CurvePrValue::posMax, juce::sendNotification);
            }
            else if (pos2x < pos1x) {
                value[p][t][vp][2]->setValue(pos1x, juce::sendNotification);
            }

            ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
            };

        auto applyChange = [this, correctPosition](int p, int t, int vp, int vv) {
            if (vv >= 0 && vv < 2) {
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp, vv, correctPosition] {
                    correctPosition(p, t, vp, vv);
                    };
            }
            else {
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp] {
                    ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
                    };
            }
            };

        auto applyChangePos = [this, correctPosition, correctPosition0, correctPosition2](int p, int t, int vp, int vv) {
            switch (vv) {
            case 0:
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp, correctPosition0] {
                    correctPosition0(p, t, vp);
                    };

                break;
            case 1:
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp, vv, correctPosition] {
                    correctPosition(p, t, vp, vv);
                    };

                break;
            case 2:
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp, correctPosition2] {
                    correctPosition2(p, t, vp);
                    };

                break;
            case 3:
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp, vv, correctPosition] {
                    correctPosition(p, t, vp, vv);
                    };

                break;
            default:
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp] {
                    ctx.audioProcessor.bakeCurvesPrim(p, t, vp);
                    };
            }
            };

        auto applyRangeEventByLogic = [this, applyChange, applyChangePos](int currentLogic, int p, int t, int vp, int vv) {
            // posの範囲チェックが必要なロジック
            switch ((CurveParams::Logic)currentLogic) {
            case CurveParams::Logic::LinearArcExp:  // 7 | 線形 + 円弧(指数的) | pos1.x, pos1.y
            case CurveParams::Logic::LinearArcLog:  // 8 | 線形 + 円弧(対数的) | pos1.x, pos1.y
            case CurveParams::Logic::LinearExp:     // 9 | 線形 + 指数 | pos1.x, pos1.y, rate
            case CurveParams::Logic::LinearLog:     // 10 | 線形 + 対数 | pos1.x, pos1.y, rate
            case CurveParams::Logic::LinearSp1:     // 11 | 線形 + 1点スプライン | pos1.x, pos1.y, cp1.x, cp1.y
            case CurveParams::Logic::LinearSp2:     // 12 | 線形 + 2点スプライン | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y
            case CurveParams::Logic::ArcExpLinear:  // 13 | 円弧(指数的) + 線形 | pos1.x, pos1.y
            case CurveParams::Logic::ArcLogLinear:  // 14 | 円弧(対数的) + 線形 | pos1.x, pos1.y
            case CurveParams::Logic::ExpLinear:     // 15 | 指数 + 線形 | pos1.x, pos1.y, rate
            case CurveParams::Logic::LogLinear:     // 16 | 対数 + 線形 | pos1.x, pos1.y, rate
            case CurveParams::Logic::Sp1Linear:     // 17 | 1点スプライン + 線形 | pos1.x, pos1.y, cp1.x, cp1.y
            case CurveParams::Logic::Sp2Linear:     // 18 | 2点スプライン + 線形 | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y
                applyChange(p, t, vp, vv);

                break;
            case CurveParams::Logic::Linear2ArcExp: // 19 | 線形 + 円弧(指数的) + 線形 | pos1.x, pos1.y, pos2.x, pos2.y
            case CurveParams::Logic::Linear2ArcLog: // 20 | 線形 + 円弧(対数的) + 線形 | pos1.x, pos1.y, pos2.x, pos2.y
            case CurveParams::Logic::Linear2Exp:    // 21 | 線形 + 指数 + 線形 | pos1.x, pos1.y, pos2.x, pos2.y, rate
            case CurveParams::Logic::Linear2Log:    // 22 | 線形 + 対数 + 線形 | pos1.x, pos1.y, pos2.x, pos2.y, rate
            case CurveParams::Logic::Linear2Sp1:    // 23 | 線形 + 1点スプライン + 線形 | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y
            case CurveParams::Logic::Linear2Sp2:    // 24 | 線形 + 2点スプライン + 線形 | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y
                applyChangePos(p, t, vp, vv);

                break;
            case CurveParams::Logic::Linear2:        // 25 | 線形 + 線形 | pos1.x, pos1.y
                applyChange(p, t, vp, vv);

                break;
            case CurveParams::Logic::Linear3:        // 26 | 線形 + 線形 + 線形 | pos1.x, pos1.y, pos2.x, pos2.y
                applyChangePos(p, t, vp, vv);

                break;
            case CurveParams::Logic::Sprine12:        // 27 | 1点スプライン + 1点スプライン | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y
            case CurveParams::Logic::Sprine22:        // 28 | 2点スプライン + 2点スプライン | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y, cp3.x, cp3.y, cp4.x, cp4.y
                applyChange(p, t, vp, vv);

                break;
            case CurveParams::Logic::Sprine13:        // 29 | 1点スプライン + 1点スプライン + 1点スプライン | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y, cp3.x, cp3.y
            case CurveParams::Logic::Sprine23:        // 30 | 2点スプライン + 2点スプライン + 2点スプライン | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y, cp3.x, cp3.y, cp4.x, cp4.y, cp5.x, cp5.y, cp6.x, cp6.y
                applyChangePos(p, t, vp, vv);

                break;
            default:
                value[p][t][vp][vv]->onValueChange = [this, p, t, vp] {
                    ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // 値が変わるとカーブの形状が変わるので、値変更時もカーブをベイクする
                    };

                break;
            }
            };

        int p = position.getSelectedItemIndex();
        int t = target.getSelectedItemIndex();
        for (int vp = 0; vp < CurvePrValue::params; vp++) {
            int currentLogic = logic[p][t][vp]->getSelectedItemIndex();

            for (int vv = 0; vv < CurvePrValue::values; vv++) {
                setRangeByLogic(currentLogic, p, t, vp, vv);
                applyRangeEventByLogic(currentLogic, p, t, vp, vv);
            }
        }
        };

    const juce::String code = CurvePrKey::prefix;
    int tabOrder = 1;

    curveGroup.setup(*this, CurveGuiText::curveGroup);

    enable.setup({ .parent = *this, .id = code + CurvePrKey::enable, .title = CurveGuiText::enable, .isReset = true });
    enable.setWantsKeyboardFocus(true);
    enable.setExplicitFocusOrder(++tabOrder);
    enable.onStateChange = [this] {
        updateEnabled();
        updateVisible();
        ctx.editor.resized();
        };

    position.setup({ .parent = *this, .id = "", .title = CurveGuiText::position, .items = positionItems, .isReset = false });
    position.setSelectedItemIndex(0);
    position.setWantsKeyboardFocus(true);
    position.setExplicitFocusOrder(++tabOrder);
    position.onChange = [this, correctTarget] {
        updateVisible();

        correctTarget();

        ctx.editor.resized();
        };

    target.setup({ .parent = *this, .id = "", .title = CurveGuiText::target, .items = targetItems, .isReset = false });
    target.setSelectedItemIndex(0);
    target.setWantsKeyboardFocus(true);
    target.setExplicitFocusOrder(++tabOrder);
    target.onChange = [this] {
        updateVisible();
        ctx.editor.resized();
        };

    correctTarget();

    mainSeparator.setupComponent(*this);

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp]->setup({ .parent = curveGroup.contentCanvas, .title = "" });
        paramLabel[vp]->setWantsKeyboardFocus(true);
        paramLabel[vp]->setExplicitFocusOrder(++tabOrder);

        curveGroup.contentCanvas.addAndMakeVisible(curveGraphs[vp].get());
    }

    for (int p = 0; p < CurvePrValue::positions; p++) {
        const juce::String pCode = code + CurvePrKey::position[p];

        for (int t = 0; t < CurvePrValue::targets; t++) {
            const juce::String tCode = pCode + CurvePrKey::target[t];

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                const juce::String vpCode = tCode + CurvePrKey::paramList[vp];

                logic[p][t][vp]->setup({.parent = curveGroup.contentCanvas, .id = vpCode + CurvePrKey::logic, .title = CurveGuiText::logic, .items = logicItems, .isReset = true});
                logic[p][t][vp]->setWantsKeyboardFocus(true);
                logic[p][t][vp]->setExplicitFocusOrder(++tabOrder);
                logic[p][t][vp]->onChange = [this, p, t, vp, setRangeByLogic] {
                    int currentLogic = logic[p][t][vp]->getSelectedItemIndex();

                    for (int vv = 0; vv < CurvePrValue::values; vv++) {
                        setRangeByLogic(currentLogic, p, t, vp, vv);
                    }

                    updateVisible();
                    ctx.editor.resized();
					ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // カーブのロジックが変わるので、ロジック変更時もカーブをベイクする
                    };

				int currentLogic = logic[p][t][vp]->getSelectedItemIndex();

                k[p][t][vp]->setup({ .parent = curveGroup.contentCanvas, .id = vpCode + CurvePrKey::k, .title = CurveGuiText::k, .isReset = true });
                k[p][t][vp]->setWantsKeyboardFocus(true);
                k[p][t][vp]->setExplicitFocusOrder(++tabOrder);
                k[p][t][vp]->onValueChange = [this, p, t, vp] {
					ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // Kの値が変わるとカーブの形状が変わるので、K変更時もカーブをベイクする
                    };

                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    const juce::String vvCode = vpCode + CurvePrKey::valueList[vv];

                    value[p][t][vp][vv]->setup({ .parent = curveGroup.contentCanvas, .id = vvCode, .title = "", .isReset = true});
                    value[p][t][vp][vv]->setWantsKeyboardFocus(true);
                    value[p][t][vp][vv]->setExplicitFocusOrder(++tabOrder);
                    value[p][t][vp][vv]->onValueChange = [this, p, t, vp] {
						ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // 値が変わるとカーブの形状が変わるので、値変更時もカーブをベイクする
                        };

                    setRangeByLogic(currentLogic, p, t, vp, vv);
                }
            }
        }
    }

    updateEnabled();
    updateVisible();
}

void GuiCurve::layout(juce::Rectangle<int> content)
{
    const int scrollHeight = 3000;
    const int graphWidth = 400;
	const int graphHeight = 400;
    juce::String code = CurvePrKey::prefix;
    auto pageArea = content.withZeroOrigin();
    int p = position.getSelectedItemIndex();
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
    int mainWidth = baseWidth / 3;
    int valueWidth = (baseWidth - graphWidth) / 2;

    auto enRect = mmRect.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

    enable.setBounds(enRect.removeFromLeft(titleWidth).reduced(px, py));

    auto pRect = enRect.removeFromLeft(mainWidth);

    position.label.setBounds(pRect.removeFromLeft(lw).reduced(px, py));
    position.setBounds(pRect.reduced(px, py));

    auto tRect = enRect.removeFromLeft(mainWidth);

    target.label.setBounds(tRect.removeFromLeft(lw).reduced(px, py));
    target.setBounds(tRect.reduced(px, py));

    mainSeparator.layoutComponent(mmRect);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    curveGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, curveGroup.viewport.getMaximumVisibleWidth(), scrollHeight);

    auto layoutValue = [this, titleWidth, valueWidth, lw, px, py](juce::Rectangle<int>& valueArea, int logicNum, int valNum, int p, int t, int vp, int vv) {
        auto vvRect = valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);
        int x = vv % 2;

        vvRect.removeFromLeft(titleWidth);

        auto vv1Rect = vvRect.removeFromLeft(valueWidth);

        value[p][t][vp][vv]->label.setText(valueLabels[logicNum][vv], juce::dontSendNotification);
        value[p][t][vp][vv]->label.setBounds(vv1Rect.removeFromLeft(lw).reduced(px, py));
        value[p][t][vp][vv]->setBounds(vv1Rect.reduced(px, py));

        if (vv + 1 == valNum) {
            return;
        }

        auto vv2Rect = vvRect.removeFromLeft(valueWidth);

        value[p][t][vp][vv + 1]->label.setText(valueLabels[logicNum][vv + 1], juce::dontSendNotification);
        value[p][t][vp][vv + 1]->label.setBounds(vv2Rect.removeFromLeft(lw).reduced(px, py));
        value[p][t][vp][vv + 1]->setBounds(vv2Rect.reduced(px, py));

        if (vv + 2 == valNum) {
            return;
        }

        valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::valuePaddingY);
        };

    for (int vp = 0; vp < vpLen; vp++) {
        auto paramArea = mRect.removeFromTop(graphHeight);

        auto valueArea = paramArea.removeFromLeft(paramArea.getWidth() - graphWidth);

        auto vpRect = valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

        auto vpLbRect = vpRect.removeFromLeft(titleWidth);

        paramLabel[vp]->setText(paramTitles[t][vp], juce::NotificationType::dontSendNotification);
        paramLabel[vp]->setBounds(vpLbRect.reduced(px, py));

        auto logicRect = vpRect.removeFromLeft(valueWidth);

        logic[p][t][vp]->label.setBounds(logicRect.removeFromLeft(lw).reduced(px, py));
        logic[p][t][vp]->setBounds(logicRect.reduced(px, py));

        auto kRect = vpRect.removeFromLeft(valueWidth);

        k[p][t][vp]->label.setBounds(kRect.removeFromLeft(lw).reduced(px, py));
        k[p][t][vp]->setBounds(kRect.reduced(px, py));

        int logicNum = logic[p][t][vp]->getSelectedItemIndex();
        int values = valueLengthes[logicNum];

        valueArea.removeFromTop(CurveGuiValue::CurveGroup::Row::valueSpacingY);

        for (int vv = 0; vv < values; vv += 2) {
			layoutValue(valueArea, logicNum, values, p, t, vp, vv);
        }

        curveGraphs[vp]->setVisible(vp < vpLen);

        auto graphArea = paramArea.reduced(20, 20);

        curveGraphs[vp]->setBounds(graphArea);

        if (vp < vpLen - 1) {
            mRect.removeFromTop(CurveGuiValue::CurveGroup::Row::valuePaddingY);
        }
    }

    int usedHeight = scrollHeight - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    curveGroup.setContentHeight(usedHeight);
}

void GuiCurve::updateEnabled() {
    bool enabled = enable.getToggleState();
    int t = target.getSelectedItemIndex();
    int vpLen = paramLengthes[t];

    position.setEnabled(enabled);
    position.label.setEnabled(enabled);

    target.setEnabled(enabled);
    target.label.setEnabled(enabled);

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp]->setEnabled(enabled);
        curveGraphs[vp]->setEnabled(enabled);
    }

    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                logic[p][t][vp]->setEnabled(enabled);
                logic[p][t][vp]->label.setEnabled(enabled);

                k[p][t][vp]->setEnabled(enabled);
                k[p][t][vp]->label.setEnabled(enabled);

                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    value[p][t][vp][vv]->setEnabled(enabled);
                    value[p][t][vp][vv]->label.setEnabled(enabled);
                }
            }
        }
    }
}

void GuiCurve::updateVisible()
{
    int selectedPosition = position.getSelectedItemIndex();
    int selectedTarget = target.getSelectedItemIndex();
    int vpLen = paramLengthes[selectedTarget];

    bool baseVisible = false;

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        bool paramVisible = (vp < vpLen);

        paramLabel[vp]->setVisible(paramVisible);
    }

    for (int p = 0; p < CurvePrValue::positions; p++) {
        for (int t = 0; t < CurvePrValue::targets; t++) {
            baseVisible = (p == selectedPosition) && (t == selectedTarget);

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                bool paramVisible = baseVisible && (vp < vpLen);

                logic[p][t][vp]->label.setVisible(paramVisible);
                logic[p][t][vp]->setVisible(paramVisible);

                k[p][t][vp]->label.setVisible(paramVisible);
                k[p][t][vp]->setVisible(paramVisible);

                int logicNum = logic[p][t][vp]->getSelectedItemIndex();
                int valueLen = valueLengthes[logicNum];

                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    bool valueVisible = paramVisible && (vv < valueLen);

                    value[p][t][vp][vv]->label.setVisible(valueVisible);
                    value[p][t][vp][vv]->setVisible(valueVisible);
                }
            }
        }
    }

    // グラフを現在の選択状態にバインドする
    std::vector<GuiSlider*> activeSliders;
    GuiSlider* activeKSlider = nullptr;
    int currentLogicNum = 0;

    for (int vp = 0; vp < vpLen; vp++) {
        if (paramLabel[vp]->isVisible()) {
            currentLogicNum = logic[selectedPosition][selectedTarget][vp]->getSelectedItemIndex();
            int valueLen = valueLengthes[currentLogicNum];

            activeKSlider = k[selectedPosition][selectedTarget][vp].get(); // Kを取得

            for (int vv = 0; vv < valueLen; vv++) {
                activeSliders.push_back(value[selectedPosition][selectedTarget][vp][vv].get());
            }
            break;
        }
    }

    for (int vp = 0; vp < vpLen; vp++) {
        if (!paramLabel[vp]->isVisible()) continue;

        int currentLogicNum = logic[selectedPosition][selectedTarget][vp]->getSelectedItemIndex();
        int valueLen = valueLengthes[currentLogicNum];

        std::vector<GuiSlider*> activeSliders;
        GuiSlider* activeKSlider = k[selectedPosition][selectedTarget][vp].get();

        for (int vv = 0; vv < valueLen; vv++) {
            activeSliders.push_back(value[selectedPosition][selectedTarget][vp][vv].get());
        }

        // タイトルとして "AR" 等のパラメータ名を渡してバインド
        juce::String graphTitle = paramTitles[selectedTarget][vp];
        curveGraphs[vp]->bindActiveParameters(graphTitle, currentLogicNum, activeSliders, activeKSlider);

        // スライダーが動いたら「自分自身のグラフだけ」を再描画するようにコールバックを更新
        auto graphPtr = curveGraphs[vp].get();
        for (auto* sl : activeSliders) {
            // ※既存のコールバックを上書きするため、ラムダでキャプチャするgraphPtrを使います
            sl->onValueChange = [graphPtr]() { graphPtr->repaint(); };
        }
        if (activeKSlider != nullptr) {
            activeKSlider->onValueChange = [graphPtr]() { graphPtr->repaint(); };
        }
    }
}

void GuiCurve::initParams()
{
    this->ctx.audioProcessor.initParams("ADV_CURVE_");
}
