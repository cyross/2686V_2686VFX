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
    {.name = juce::String("") + "線形",                        .value = 1 },
    {.name = juce::String("") + "円弧(指数的)",                .value = 2 },
    {.name = juce::String("") + "円弧(対数的)",                .value = 3 },
    {.name = juce::String("") + "指数関数",                    .value = 4 },
    {.name = juce::String("") + "対数関数",                    .value = 5 },
    {.name = juce::String("") + "1点スプライン",               .value = 6 },
    {.name = juce::String("") + "2点スプライン",               .value = 7 },
    {.name = juce::String("") + "線形 + 円弧(指数的)",         .value = 8 },
    {.name = juce::String("") + "線形 + 円弧(対数的)",         .value = 9 },
    {.name = juce::String("") + "線形 + 指数関数",             .value = 10 },
    {.name = juce::String("") + "線形 + 対数関数",             .value = 11 },
    {.name = juce::String("") + "線形 + 1点スプライン",        .value = 12 },
    {.name = juce::String("") + "線形 + 2点スプライン",        .value = 13 },
    {.name = juce::String("") + "円弧(指数的) + 線形",         .value = 14 },
    {.name = juce::String("") + "円弧(対数的) + 線形",         .value = 15 },
    {.name = juce::String("") + "指数関数 + 線形",             .value = 16 },
    {.name = juce::String("") + "対数関数 + 線形",             .value = 17 },
    {.name = juce::String("") + "1点スプライン + 線形",        .value = 18 },
    {.name = juce::String("") + "2点スプライン + 線形",        .value = 19 },
    {.name = juce::String("") + "線形 + 円弧(指数的) + 線形",  .value = 20 },
    {.name = juce::String("") + "線形 + 円弧(対数的) + 線形",  .value = 21 },
    {.name = juce::String("") + "線形 + 指数関数 + 線形",      .value = 22 },
    {.name = juce::String("") + "線形 + 対数関数 + 線形",      .value = 23 },
    {.name = juce::String("") + "線形 + 1点スプライン + 線形", .value = 24 },
    {.name = juce::String("") + "線形 + 2点スプライン + 線形", .value = 25 }
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
    6, 8  // Linear2Sp1 Linear2Sp2
};

static std::vector<std::vector<juce::String>> valueLabels = {
    {}, // Linear
    {}, // ArcExp
    {}, // ArcLog
    {"Rate"}, // Exp
    {"Rate"}, // Log
    {"ControlPoint X", "ControlPoint Y"}, // Sp1
    {"ControlPoint1 X", "ControlPoint1 Y", "ControlPoint2 X", "ControlPoint2 Y"}, // Sp2
    {"ViaPoint X", "ViaPoint Y"}, // LinearArcExp
    {"ViaPoint X", "ViaPoint Y"}, // LinearArcLog
    {"ViaPoint X", "ViaPoint Y", "Rate"}, // LinearExp
    {"ViaPoint X", "ViaPoint Y", "Rate"}, // LinearLog
    {"ViaPoint X", "ViaPoint Y", "ControlPoint X", "ControlPoint Y"}, // LinearSp1
    {"ViaPoint X", "ViaPoint Y", "ControlPoint1 X", "ControlPoint1 Y", "ControlPoint2 X", "ControlPoint2 Y"}, // LinearSp2
    {"ViaPoint X", "ViaPoint Y"}, // ArcExpLinear
    {"ViaPoint X", "ViaPoint Y"}, // ArcLogLinear
    {"Rate", "ViaPoint X", "ViaPoint Y"}, // ExpLinear
    {"Rate", "ViaPoint X", "ViaPoint Y"}, // LogLinear
    {"ControlPoint X", "ControlPoint Y", "ViaPoint X", "ViaPoint Y"}, // Sp1Linear
    {"ControlPoint1 X", "ControlPoint1 Y", "ControlPoint2 X", "ControlPoint2 Y", "ViaPoint X", "ViaPoint Y"}, // Sp2Linear
    {"ViaPoint1 X", "ViaPoint1 Y", "ViaPoint2 X", "ViaPoint2 Y"}, // Linear2ArcExp
    {"ViaPoint1 X", "ViaPoint1 Y", "ViaPoint2 X", "ViaPoint2 Y"}, // Linear2ArcLog
    {"ViaPoint1 X", "ViaPoint1 Y", "ViaPoint2 X", "ViaPoint2 Y", "Rate"}, // Linear2Exp
    {"ViaPoint1 X", "ViaPoint1 Y", "ViaPoint2 X", "ViaPoint2 Y", "Rate"}, // Linear2Log
    {"ViaPoint1 X", "ViaPoint1 Y", "ViaPoint2 X", "ViaPoint2 Y", "ControlPoint X", "ControlPoint Y"}, // Linear2Sp1
    {"ViaPoint1 X", "ViaPoint1 Y", "ViaPoint2 X", "ViaPoint2 Y", "ControlPoint1 X", "ControlPoint1 Y", "ControlPoint2 X", "ControlPoint2 Y"}  // Linear2Sp2
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
    const juce::String code = CurvePrKey::prefix;
    int tabOrder = 1;

    curveGroup.setup(*this, CurveGuiText::curveGroup);

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        addAndMakeVisible(curveGraphs[vp].get());
    }

    enable.setup({ .parent = *this, .id = code + CurvePrKey::enable, .title = CurveGuiText::enable, .isReset = true });
    enable.setWantsKeyboardFocus(true);
    enable.setExplicitFocusOrder(++tabOrder);
    enable.onStateChange = [this] {
        updateEnabled();
        updateVisible();
        ctx.editor.resized();
        };

    position.setup({ .parent = *this, .id = "", .title = CurveGuiText::position, .items = positionItems, .isReset = false});
    position.setSelectedItemIndex(0);
    position.setWantsKeyboardFocus(true);
    position.setExplicitFocusOrder(++tabOrder);
    position.onChange = [this] {
        updateVisible();
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

    addAndMakeVisible(mainSeparator);
    mainSeparator.setup({ .lineRate = 0.8f, .lineThick = 4.0f, .lineColour = juce::Colours::grey });

    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        paramLabel[vp]->setup({ .parent = *this, .title = "" });
        paramLabel[vp]->setWantsKeyboardFocus(true);
        paramLabel[vp]->setExplicitFocusOrder(++tabOrder);
    }

    for (int p = 0; p < CurvePrValue::positions; p++) {
        const juce::String pCode = code + CurvePrKey::position[p];

        for (int t = 0; t < CurvePrValue::targets; t++) {
            const juce::String tCode = pCode + CurvePrKey::target[t];

            for (int vp = 0; vp < CurvePrValue::params; vp++) {
                const juce::String vpCode = tCode + CurvePrKey::paramList[vp];

                logic[p][t][vp]->setup({.parent = *this, .id = vpCode + CurvePrKey::logic, .title = CurveGuiText::logic, .items = logicItems, .isReset = true});
                logic[p][t][vp]->setWantsKeyboardFocus(true);
                logic[p][t][vp]->setExplicitFocusOrder(++tabOrder);
                logic[p][t][vp]->onChange = [this, p, t, vp] {
                    updateVisible();
                    ctx.editor.resized();
					ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // カーブのロジックが変わるので、ロジック変更時もカーブをベイクする
                    };

                k[p][t][vp]->setup({ .parent = *this, .id = vpCode + CurvePrKey::k, .title = CurveGuiText::k, .isReset = true });
                k[p][t][vp]->setWantsKeyboardFocus(true);
                k[p][t][vp]->setExplicitFocusOrder(++tabOrder);
                k[p][t][vp]->onValueChange = [this, p, t, vp] {
					ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // Kの値が変わるとカーブの形状が変わるので、K変更時もカーブをベイクする
                    };

                for (int vv = 0; vv < CurvePrValue::values; vv++) {
                    const juce::String vvCode = vpCode + CurvePrKey::valueList[vv];

                    value[p][t][vp][vv]->setup({ .parent = *this, .id = vvCode, .title = "", .isReset = true});
                    value[p][t][vp][vv]->setWantsKeyboardFocus(true);
                    value[p][t][vp][vv]->setExplicitFocusOrder(++tabOrder);
                    value[p][t][vp][vv]->onValueChange = [this, p, t, vp] {
						ctx.audioProcessor.bakeCurvesPrim(p, t, vp); // 値が変わるとカーブの形状が変わるので、値変更時もカーブをベイクする
                        };
                }
            }
        }
    }

    updateEnabled();
    updateVisible();
}

void GuiCurve::layout(juce::Rectangle<int> content)
{
    juce::String code = CurvePrKey::prefix;
    auto pageArea = content.withZeroOrigin();
    int p = position.getSelectedItemIndex();
    int t = target.getSelectedItemIndex();
    int vpLen = paramLengthes[t];
    int px = CurveGuiValue::CurveGroup::Row::Padding::x;
    int py = CurveGuiValue::CurveGroup::Row::Padding::y;
    int lw = CurveGuiValue::CurveGroup::Row::labelWidth;

    curveGroup.setBounds(pageArea);

    auto mainArea = pageArea.reduced(CurveGuiValue::CurveGroup::Padding::width, CurveGuiValue::CurveGroup::Padding::height);

    auto rightGraphArea = mainArea.removeFromRight(400).reduced(50, 50);

    auto graphSize = rightGraphArea.getWidth() / 2;
    int graphYItems = 4;

    // グラフを表示しないものは非表示にしておく
    for (int vp = 0; vp < CurvePrValue::params; vp++) {
        curveGraphs[vp]->setVisible(vp < vpLen);
    }

    if (vpLen > 0) {
        int graphXItems = vpLen / graphYItems;

        for (int x = 0; x <= graphXItems; x++) {
            auto bRect = rightGraphArea.removeFromLeft(graphSize);

            for (int y = 0; y < 4; y++) {
                int vp = x * 4 + y;

                if (vp < vpLen) {
                    auto rect = bRect.removeFromTop(graphSize).reduced(5);
                    curveGraphs[vp]->setBounds(rect);
                }
            }
        }
    }

    mainArea.removeFromTop(CurveGuiValue::CurveGroup::Padding::height);

    int titleWidth = CurveGuiValue::CurveGroup::Row::titleWidth;
    int baseWidth = mainArea.getWidth() - titleWidth;
    int mainWidth = baseWidth / 3;
    int valueWidth = baseWidth / 3;

    auto enRect = mainArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

    enable.setBounds(enRect.removeFromLeft(titleWidth).reduced(px, py));

    auto pRect = enRect.removeFromLeft(mainWidth);

    position.label.setBounds(pRect.removeFromLeft(lw).reduced(px, py));
    position.setBounds(pRect.reduced(px, py));

    auto tRect = enRect.removeFromLeft(mainWidth);

    target.label.setBounds(tRect.removeFromLeft(lw).reduced(px, py));
    target.setBounds(tRect.reduced(px, py));

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mainArea.removeFromTop(CurveGuiValue::CurveGroup::Separator::height);
    mainSeparator.setBounds(presetNameSeparatorArea);

    for (int vp = 0; vp < vpLen; vp++) {
        auto vpRect = mainArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

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

        auto vvRect = mainArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

        vvRect.removeFromLeft(titleWidth);

        for (int vv = 0; vv < values; vv++) {
            int x = vv % 3;

            auto vvvRect = vvRect.removeFromLeft(valueWidth);

            value[p][t][vp][vv]->label.setText(valueLabels[logicNum][vv], juce::dontSendNotification);
            value[p][t][vp][vv]->label.setBounds(vvvRect.removeFromLeft(lw).reduced(px, py));
            value[p][t][vp][vv]->setBounds(vvvRect.reduced(px, py));

            if (x == 2 && vv < values - 1)
            {
                mainArea.removeFromTop(CurveGuiValue::CurveGroup::Row::valuePaddingY);

                vvRect = mainArea.removeFromTop(CurveGuiValue::CurveGroup::Row::height);

                vvRect.removeFromLeft(titleWidth);
            }
        }

        if (vp < vpLen - 1) {
            mainArea.removeFromTop(CurveGuiValue::CurveGroup::Row::valuePaddingY);
        }
    }
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
