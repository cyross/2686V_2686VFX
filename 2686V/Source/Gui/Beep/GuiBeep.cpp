#include "./GuiBeep.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Beep/ProcessorBeepKeys.h"
#include "../../Processor/Beep/ProcessorBeepValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiBeepValues.h"
#include "./GuiBeepText.h"

void GuiBeep::setup() {
    juce::String code = BeepPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, BeepGuiText::Group::mainGroup); // GuiText 等に置換

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    volSlider.setup({ .parent = *this, .id = code + BeepPrKey::level, .title = BeepGuiText::Beep::Level, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

	fixComponent.setupComponent(*this, code, tabOrder, "-> 2K", 2000);

    ampEnvComponent.setupComponent(*this, code, tabOrder);

    pitchEnvComponent.setupComponent(*this, code, tabOrder, BeepPrKey::pitchAdsr + BeepPrKey::bypass, BeepGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(*this, code, tabOrder, BeepPrKey::ssgSwEnv + BeepPrKey::bypass, BeepGuiText::SsgSwEnv::bypass);

	mulDetuneComponent.setupComponent(*this, code, tabOrder);

    unisonComponent.setupComponent(*this, code, tabOrder);

    midiComponent.setupComponent(*this, tabOrder);

    setupGraph();
    updateGraph();
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(BeepGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(BeepGuiValue::Group::Padding::width, BeepGuiValue::Group::Padding::height);
    mRect.removeFromTop(BeepGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(BeepGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // グラフ用の区画を確保
    layoutGraph(mRect);
    updateGraph();

    layoutMain({ .mainRect = mRect, .label = &volSlider.label, .component = &volSlider });

    fixComponent.layoutComponent(mRect);

    ampEnvComponent.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

	mulDetuneComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    midiComponent.layoutComponent(mRect);
}

void GuiBeep::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiBeep::initParams()
{
    this->ctx.audioProcessor.initParams("BEEP_");
}

void GuiBeep::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    auto repaintGraph = [this]() { updateGraph(); };
    
    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator);
    graphSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiBeep::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiBeep::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(BeepGuiValue::MainGroup::Graph::height + BeepGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(BeepGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(BeepGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiBeep::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        ampEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
}
