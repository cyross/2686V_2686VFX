#include "./GuiBeep.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Beep/ProcessorBeepKeys.h"
#include "../../Processor/Beep/ProcessorBeepValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiBeepValues.h"
#include "./GuiBeepText.h"

void GuiBeep::setup() {
    juce::String code = BeepPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, BeepGuiText::Group::mainGroup); // GuiText 等に置換

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

	fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 2K", 2000);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, BeepGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, BeepGuiText::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, BeepGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, BeepGuiText::SsgSwPEnv11::bypass);

	mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = BeepGuiText::Category::visibleUtil, .invisibleTitle = BeepGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = BeepGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

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

    ieSsgSwEnv11.setupComponent(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieSsgSwEnv11.onClickImport = [this] { importSsgSwEnv11Param(); };
    ieSsgSwEnv11.onClickExport = [this] { exportSsgSwEnv11Param(); };

    ieSsgSwPEnv11.setupComponent(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieSsgSwPEnv11.onClickImport = [this] { importSsgSwPEnv11Param(); };
    ieSsgSwPEnv11.onClickExport = [this] { exportSsgSwPEnv11Param(); };

    ieUnison.setupComponent(mainGroup.contentCanvas, tabOrder, "Unison");
    ieUnison.onClickImport = [this] { importUnisonParam(); };
    ieUnison.onClickExport = [this] { exportUnisonParam(); };

    setupGraph();
    updateGraph();
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(BeepGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
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

    levelComponent.layoutComponent(mRect);

    ampEnvComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    ssgSwEnv11Component.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwPEnv11Component.layoutComponent(mRect);

	mulDetuneComponent.layoutComponent(mRect);

    lfoComponent.layoutComponent(mRect);

    fixComponent.layoutComponent(mRect);

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
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieUnison.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieAmpEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        iePitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwPEnv11.layoutComponent(rect);
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

    graphBtnAmp.setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "PIT", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    graphBtnSsg11.setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    graphBtnSsg11.onClick = [this] { setGraphMode(GraphMode::SsgSw11); };

    graphBtnSsgP11.setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    graphBtnSsgP11.onClick = [this] { setGraphMode(GraphMode::SsgSwP11); };

    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return;

        this->isUpdatingGraph = true;
        this->updateGraph();
        this->isUpdatingGraph = false;
        };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    ssgSwEnv11Component.setupGraph(repaintGraph);

    ssgSwPEnv11Component.setupGraph(repaintGraph);

    graphSeparator.setupComponent(*this);
}

void GuiBeep::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GraphMode::SsgSwP11, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiBeep::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(BeepGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(BeepGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 5;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsgP11.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiBeep::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11Component.updateGraph(graph);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11Component.updateGraph(graph);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        ampEnvComponent.updateGraph(graph);
    }
}

void GuiBeep::setLevel(float level) {
    levelComponent.setLevel(level);
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

void GuiBeep::importSsgSwEnv11Param() {
    ssgSwEnv11Component.importParams();
}

void GuiBeep::exportSsgSwEnv11Param() {
    ssgSwEnv11Component.exportParams();
}

void GuiBeep::importSsgSwPEnv11Param() {
    ssgSwPEnv11Component.importParams();
}

void GuiBeep::exportSsgSwPEnv11Param() {
    ssgSwPEnv11Component.exportParams();
}
