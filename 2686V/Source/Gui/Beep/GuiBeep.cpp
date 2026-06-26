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

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + BeepPrKey::level, .title = BeepGuiText::Beep::Level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

	fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 2K", 2000);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, BeepPrKey::pitchAdsr + BeepPrKey::bypass, BeepGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, BeepPrKey::ssgSwEnv + BeepPrKey::bypass, BeepGuiText::SsgSwEnv::bypass);

	mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = BeepGuiText::Category::visibleUtil, .invisibleTitle = BeepGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = BeepGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    mainGroup.contentCanvas.addAndMakeVisible(uSep001);
    uSep001.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    ieLfo.setupComponent(mainGroup.contentCanvas, tabOrder, "LFO");
    ieLfo.onClickImport = [this] { importLfoParam(); };
    ieLfo.onClickExport = [this] { exportLfoParam(); };

    ieDetune.setupComponent(mainGroup.contentCanvas, tabOrder, "Detune");
    ieDetune.onClickImport = [this] { importDetuneParam(); };
    ieDetune.onClickExport = [this] { exportDetuneParam(); };

    ieAmpEnv.setupComponent(mainGroup.contentCanvas, tabOrder, "Amp Env");
    ieAmpEnv.onClickImport = [this] { importAmpEnvParam(); };
    ieAmpEnv.onClickExport = [this] { exportAmpEnvParam(); };

    iePitchEnv.setupComponent(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    iePitchEnv.onClickImport = [this] { importPitchEnvParam(); };
    iePitchEnv.onClickExport = [this] { exportPitchEnvParam(); };

    ieSsgSwEnv.setupComponent(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieSsgSwEnv.onClickImport = [this] { importSsgSwEnvParam(); };
    ieSsgSwEnv.onClickExport = [this] { exportSsgSwEnvParam(); };

    ieUnison.setupComponent(mainGroup.contentCanvas, tabOrder, "Unison");
    ieUnison.onClickImport = [this] { importUnisonParam(); };
    ieUnison.onClickExport = [this] { exportUnisonParam(); };


    setupGraph();
    updateGraph();
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(BeepGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mmRect = mainArea.reduced(BeepGuiValue::Group::Padding::width, BeepGuiValue::Group::Padding::height);
    mmRect.removeFromTop(BeepGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフ用の区画を確保
    layoutGraph(mmRect);
    updateGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    fixComponent.layoutComponent(mRect);

    ampEnvComponent.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

	mulDetuneComponent.layoutComponent(mRect);

    lfoComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);
}

void GuiBeep::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    ieLfo.setVisible(visible);
    ieDetune.setVisible(visible);
    ieAmpEnv.setVisible(visible);
    iePitchEnv.setVisible(visible);
    ieSsgSwEnv.setVisible(visible);
    ieUnison.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });
        auto uSep001Area = rect.removeFromTop(4);
        uSep001.setBounds(uSep001Area);
        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieAmpEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        iePitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieDetune.layoutComponent(rect);
        rect.removeFromTop(4);
        ieUnison.layoutComponent(rect);
    }
}

void GuiBeep::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
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

void GuiBeep::setLevel(float level) {
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiBeep::importLfoParam() {
    lfoComponent.importParams();
}
void GuiBeep::exportLfoParam() {
    lfoComponent.exportParams();
}

void GuiBeep::importAmpEnvParam() {
    ampEnvComponent.importParams();
}

void GuiBeep::exportAmpEnvParam() {
    ampEnvComponent.exportParams();
}

void GuiBeep::importPitchEnvParam() {
    pitchEnvComponent.importParams();
}

void GuiBeep::exportPitchEnvParam() {
    pitchEnvComponent.exportParams();
}

void GuiBeep::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void GuiBeep::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void GuiBeep::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void GuiBeep::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void GuiBeep::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiBeep::exportUnisonParam() {
    unisonComponent.exportParams();
}
