#include "./GuiWtPlus.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/WtPlus/ProcessorWtPlusKeys.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiWtPlusValues.h"
#include "./GuiWtPlusText.h"

// MODULATION の変調波形。実機ディスクシステムは 32 段の階段状。
static std::vector<SelectItem> wtPlusModShapeItems = {
    {.name = "0: Sine",            .value = 1 },
    {.name = "1: FDS Triangle",    .value = 2 },
    {.name = "2: FDS Saw",         .value = 3 },
    {.name = "3: FDS Reset",       .value = 4 },
    {.name = "4: FDS Pulse",       .value = 5 },
    {.name = "5: WS Sweep Up",     .value = 6 },
    {.name = "6: WS Sweep Down",   .value = 7 },
    {.name = "7: HuC6280 Wave",    .value = 8 },
};

static std::vector<SelectItem> wtPlusStepsItems = {
    {.name = juce::String("") + "段階なし",           .value =  1 },
    {.name = juce::String("") + " 16段階(  7 /   8)", .value =  2 },
    {.name = juce::String("") + " 32段階( 15 /  16)", .value =  3 },
    {.name = juce::String("") + " 64段階( 31 /  32)", .value =  4 },
    {.name = juce::String("") + "128段階( 63 /  64)", .value =  5 },
    {.name = juce::String("") + "256段階(127 / 128)", .value =  6 },
    {.name = juce::String("") + " 16段階(  8 /   7)", .value =  7 },
    {.name = juce::String("") + " 32段階( 16 /  15)", .value =  8 },
    {.name = juce::String("") + " 64段階( 32 /  31)", .value =  9 },
    {.name = juce::String("") + "128段階( 64 /  63)", .value = 10 },
    {.name = juce::String("") + "256段階(128 / 127)", .value = 11 },
};

void GuiWtPlus::setup() {
    juce::String code = WtPlusPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, WtPlusGuiText::Group::mainGroup);
    waveGroup.setup(*this, WtPlusGuiText::Group::waveGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 2K", 2000);

    // ==========================================================
    // WAVE
    // ==========================================================
    waveCat.setupHwCategory({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Category::visibleWave, .invisibleTitle = WtPlusGuiText::Category::invisibleWave, .enableChangeDetailVisible = true });

    slotSlider.setup({ .parent = waveGroup.contentCanvas, .id = code + CPK::Wt::slot, .title = WtPlusGuiText::Wt::slot, .isReset = true });
    slotSlider.setWantsKeyboardFocus(true);
    slotSlider.setExplicitFocusOrder(++tabOrder);

    interpolateButton.setup({ .parent = waveGroup.contentCanvas, .id = code + CPK::Wt::interpolate, .title = WtPlusGuiText::Wt::interpolate, .isReset = true, .isResized = true });
    interpolateButton.setWantsKeyboardFocus(true);
    interpolateButton.setExplicitFocusOrder(++tabOrder);

    stepsSelector.setup({ .parent = waveGroup.contentCanvas, .id = code + CPK::Wt::steps, .title = WtPlusGuiText::Wt::steps, .items = wtPlusStepsItems, .isReset = true, .isResized = true });
    stepsSelector.setWantsKeyboardFocus(true);
    stepsSelector.setExplicitFocusOrder(++tabOrder);

    // ==========================================================
    // WAVE MEMORY (32 スロット)
    // ==========================================================
    slotsCat.setupHwCategory({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Category::visibleSlots, .invisibleTitle = WtPlusGuiText::Category::invisibleSlots, .enableChangeDetailVisible = true });

    for (int i = 0; i < Global::WtPlus::slots; ++i) {
        slotWtBtn[i].setup({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Wt::Slots::wt, .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
        slotWtBtn[i].setWantsKeyboardFocus(true);
        slotWtBtn[i].setExplicitFocusOrder(++tabOrder);
        slotWtBtn[i].onClick = [this, i] { importSlotWave(i, false); };

        slotWt2Btn[i].setup({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Wt::Slots::wt2, .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
        slotWt2Btn[i].setWantsKeyboardFocus(true);
        slotWt2Btn[i].setExplicitFocusOrder(++tabOrder);
        slotWt2Btn[i].onClick = [this, i] { importSlotWave(i, true); };

        slotClearBtn[i].setup({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Wt::Slots::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
        slotClearBtn[i].setWantsKeyboardFocus(true);
        slotClearBtn[i].setExplicitFocusOrder(++tabOrder);
        slotClearBtn[i].onClick = [this, i] { clearSlotWave(i); };

        slotFileNameLabel[i].setup({ .parent = waveGroup.contentCanvas, .title = Io::empty });

        updateSlotFileName(i);
    }

    // ==========================================================
    // MODULATION
    // ==========================================================
    modCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Category::visibleMod, .invisibleTitle = WtPlusGuiText::Category::invisibileMod, .enableChangeDetailVisible = true });

    modEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::enable, .title = WtPlusGuiText::Wt::Mod::enable, .isReset = true, .isResized = true });
    modEnableButton.setWantsKeyboardFocus(true);
    modEnableButton.setExplicitFocusOrder(++tabOrder);

    modDepthSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::depth, .title = WtPlusGuiText::Wt::Mod::depth, .isReset = true });
    modDepthSlider.setWantsKeyboardFocus(true);
    modDepthSlider.setExplicitFocusOrder(++tabOrder);

    modSpeedSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::speed, .title = WtPlusGuiText::Wt::Mod::speed, .isReset = true });
    modSpeedSlider.setWantsKeyboardFocus(true);
    modSpeedSlider.setExplicitFocusOrder(++tabOrder);

    modShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::shape, .title = WtPlusGuiText::Wt::Mod::shape, .items = wtPlusModShapeItems, .isReset = true, .isResized = true });
    modShapeSelector.setWantsKeyboardFocus(true);
    modShapeSelector.setExplicitFocusOrder(++tabOrder);

    modWaveWtBtn.setup({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Wt::Mod::waveWt, .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
    modWaveWtBtn.setWantsKeyboardFocus(true);
    modWaveWtBtn.setExplicitFocusOrder(++tabOrder);
    modWaveWtBtn.onClick = [this] { importModWave(false); };

    modWaveWt2Btn.setup({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Wt::Mod::waveWt2, .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
    modWaveWt2Btn.setWantsKeyboardFocus(true);
    modWaveWt2Btn.setExplicitFocusOrder(++tabOrder);
    modWaveWt2Btn.onClick = [this] { importModWave(true); };

    modWaveClearBtn.setup({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Wt::Mod::waveClear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
    modWaveClearBtn.setWantsKeyboardFocus(true);
    modWaveClearBtn.setExplicitFocusOrder(++tabOrder);
    modWaveClearBtn.onClick = [this] { clearModWave(); };

    modWaveFileNameLabel.setup({ .parent = mainGroup.contentCanvas, .title = Io::empty });

    modWaveSmoothBtn.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::WtMod::waveSmooth, .title = WtPlusGuiText::Wt::Mod::waveSmooth, .isReset = true, .isResized = true });
    modWaveSmoothBtn.setWantsKeyboardFocus(true);
    modWaveSmoothBtn.setExplicitFocusOrder(++tabOrder);

    // HuC6280 モードの変調波形パラメータを引けるようにしておく
    for (int i = 0; i < 32; ++i) {
        modWaveParams[i] = ctx.audioProcessor.apvts.getParameter(code + CPK::WtMod::wave + juce::String(i));
    }

    if (ctx.audioProcessor.wtPlusModWavePath.isNotEmpty()) {
        updateModWaveFileName(juce::File(ctx.audioProcessor.wtPlusModWavePath).getFileName());
    }

    // ==========================================================
    // 共通コンポーネント
    // ==========================================================
    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, WtPlusGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, WtPlusGuiText::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, WtPlusGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, WtPlusGuiText::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    // ==========================================================
    // UTILITY
    // ==========================================================
    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Category::visibleUtil, .invisibleTitle = WtPlusGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", lfoComponent);
    ieDetune.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Detune", mulDetuneComponent);
    ieAmpEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);
    iePitchEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Pitch Env", pitchEnvComponent);
    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);
    ieSsgSwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW Env", ssgSwEnvComponent);
    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11Component);
    ieSsgSwPEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11Component);
    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    setupGraph();
    updateGraph();
}

void GuiWtPlus::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(WtPlusGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);

    auto mmRect = mainArea.reduced(WtPlusGuiValue::Group::Padding::width, WtPlusGuiValue::Group::Padding::height);
    mmRect.removeFromTop(WtPlusGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフ用の区画を確保
    layoutGraph(mmRect);
    updateGraph();

    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる。
    // 32 スロットぶんの行を積むので、高さは多めに取っておく。
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 5000);

    levelComponent.layoutComponent(mRect);

    layoutModulationCat(mRect);

    qualityComponent.layoutComponent(mRect);

    ampEnvComponent.layoutComponent(mRect);

    ssgHwEnv.layoutComponent(mRect);

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

    int usedHeight = 5000 - mRect.getHeight();

    mainGroup.setContentHeight(usedHeight + 20);

    // ==========================================================
    // 波形メモリ設定グループ
    // ==========================================================
    auto waveArea = pageArea.removeFromLeft(WtPlusGuiValue::WaveGroupWidth);
    waveArea.removeFromBottom(40);
    waveGroup.setBounds(waveArea);

    auto wgRect = waveArea.reduced(WtPlusGuiValue::Group::Padding::width, WtPlusGuiValue::Group::Padding::height);
    wgRect.removeFromTop(WtPlusGuiValue::Group::TitlePaddingTop);

    waveGroup.setViewportCustomBounds(wgRect.translated(-waveArea.getX(), -waveArea.getY()));

    // 32 行ぶん積むので、キャンバスは高めに取ってから実使用ぶんへ縮める
    juce::Rectangle<int> wRect(0, 0, waveGroup.getContentWidth(), 5000);

    layoutWaveCat(wRect);

    layoutSlotsCat(wRect);

    int waveUsedHeight = 5000 - wRect.getHeight();

    waveGroup.setContentHeight(waveUsedHeight + 20);
}

void GuiWtPlus::layoutWaveCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &waveCat });

    bool visible = waveCat.isDetailVisible();

    slotSlider.setVisibleWithLabel(visible);
    interpolateButton.setVisible(visible);
    stepsSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &slotSlider.label, .component = &slotSlider });
        layoutMain({ .mainRect = rect, .component = &interpolateButton });
        layoutMain({ .mainRect = rect, .label = &stepsSelector.label, .component = &stepsSelector, });
    }
}

void GuiWtPlus::layoutSlotsCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &slotsCat });

    bool visible = slotsCat.isDetailVisible();

    for (int i = 0; i < Global::WtPlus::slots; ++i) {
        slotWtBtn[i].setVisible(visible);
        slotWt2Btn[i].setVisible(visible);
        slotClearBtn[i].setVisible(visible);
        slotFileNameLabel[i].setVisible(visible);
    }

    if (visible)
    {
        // 1 スロット 1 行。WT / WT2 / ファイル名 / Clear を横に並べる。
        for (int i = 0; i < Global::WtPlus::slots; ++i) {
            layoutMainWtFiles({ .rect = rect,
                                .loadWtBtn = &slotWtBtn[i],
                                .loadWt2Btn = &slotWt2Btn[i],
                                .fileNameLabel = &slotFileNameLabel[i],
                                .clearBtn = &slotClearBtn[i] });
        }
    }
}

void GuiWtPlus::layoutModulationCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &modCat });

    bool visible = modCat.isDetailVisible();
    bool isMod = modEnableButton.getToggleState();

    modEnableButton.setVisible(visible);
    modDepthSlider.setVisibleWithLabel(visible);
    modSpeedSlider.setVisibleWithLabel(visible);
    modShapeSelector.setVisibleWithLabel(visible);
    modWaveWtBtn.setVisible(visible);
    modWaveWt2Btn.setVisible(visible);
    modWaveClearBtn.setVisible(visible);
    modWaveFileNameLabel.setVisible(visible);
    modWaveSmoothBtn.setVisible(visible);

    modDepthSlider.setEnabledWithLabel(isMod);
    modSpeedSlider.setEnabledWithLabel(isMod);
    modShapeSelector.setEnabledWithLabel(isMod);
    modWaveWtBtn.setEnabled(isMod);
    modWaveWt2Btn.setEnabled(isMod);
    modWaveClearBtn.setEnabled(isMod);
    modWaveFileNameLabel.setEnabled(isMod);
    modWaveSmoothBtn.setEnabled(isMod);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &modEnableButton });
        layoutMain({ .mainRect = rect, .label = &modDepthSlider.label, .component = &modDepthSlider });
        layoutMain({ .mainRect = rect, .label = &modSpeedSlider.label, .component = &modSpeedSlider, });
        layoutMain({ .mainRect = rect, .label = &modShapeSelector.label, .component = &modShapeSelector, });
        layoutMainWtFiles({ .rect = rect, .loadWtBtn = &modWaveWtBtn, .loadWt2Btn = &modWaveWt2Btn, .fileNameLabel = &modWaveFileNameLabel, .clearBtn = &modWaveClearBtn });
        layoutMain({ .mainRect = rect, .component = &modWaveSmoothBtn });
    }
}

void GuiWtPlus::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    ieLfo.setVisible(visible);
    ieDetune.setVisible(visible);
    ieAmpEnv.setVisible(visible);
    iePitchEnv.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgSwEnv.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieUnison.setVisible(visible);
    ieChParam.setVisible(visible);

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
        ieSsgHwEnv.layoutComponent(rect);
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
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);
    }
}

void GuiWtPlus::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiWtPlus::initParams()
{
    this->ctx.audioProcessor.initParams("WTPLUS_");
}

void GuiWtPlus::setupGraph()
{
    addAndMakeVisible(&graph);

    graphBtnAmp.setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification);
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

void GuiWtPlus::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GraphMode::SsgSwP11, juce::dontSendNotification);

    updateGraph();
}

void GuiWtPlus::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(WtPlusGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(WtPlusGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 5;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsgP11.setBounds(btnArea);

    graph.setBounds(mainArea);
}

void GuiWtPlus::updateGraph()
{
    GraphMode mode = currentGraphMode;

    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph);
    }
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph);
    }
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11Component.updateGraph(graph);
    }
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11Component.updateGraph(graph);
    }
    else {
        ampEnvComponent.updateGraph(graph);
    }
}

void GuiWtPlus::setLevel(float level) {
    levelComponent.setLevel(level);
}

// ==============================================================================
// 波形メモリスロットのロード / クリア
// ==============================================================================
// 波形データそのものはプロセッサが所有し、state には相対パスが保存される。
// ここでは読み込み指示とファイル名表示だけを扱う。
void GuiWtPlus::importSlotWave(int slot, bool isWt2)
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    ctx.editor.openFileChooser(
        (isWt2 ? "Load Wave Memory (.wt2) for Slot " : "Load Wave Memory (.wt) for Slot ") + juce::String(slot),
        defaultDir,
        isWt2 ? "*.wt2" : "*.wt",
        [this, slot, isWt2](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (!file.existsAsFile()) return;

            slotFileNameLabel[slot].setText("Loading...", juce::dontSendNotification);

            juce::Timer::callAfterDelay(50, [this, slot, file]()
                {
                    ctx.audioProcessor.loadWtPlusWaveFile(slot, file);
                    ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                    updateSlotFileName(slot);
                });
        }
    );
}

void GuiWtPlus::clearSlotWave(int slot)
{
    ctx.audioProcessor.unloadWtPlusWaveFile(slot);

    updateSlotFileName(slot);
}

void GuiWtPlus::updateSlotFileName(int slot)
{
    if (slot < 0 || slot >= Global::WtPlus::slots) return;

    juce::String path = ctx.audioProcessor.wtPlusWavePaths[slot];
    juce::String name = path.isEmpty() ? Io::empty : juce::File(path).getFileName();

    // 先頭にスロット番号を出して、どの行がどのスロットか分かるようにする
    slotFileNameLabel[slot].setText(juce::String(slot).paddedLeft('0', 2) + ": " + name,
        juce::dontSendNotification);
}

// ==============================================================================
// HuC6280 モード用の変調波形
// ==============================================================================
void GuiWtPlus::importModWave(bool isWt2)
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    ctx.editor.openFileChooser(
        isWt2 ? "Load Mod Wave (.wt2)" : "Load Mod Wave (.wt)",
        defaultDir,
        isWt2 ? "*.wt2" : "*.wt",
        [this, isWt2](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (!file.existsAsFile()) return;

            updateModWaveFileName("Loading...");

            juce::Timer::callAfterDelay(50, [this, isWt2, file]()
                {
                    if (!applyModWaveFile(file, isWt2)) {
                        updateModWaveFileName(Io::empty);
                        return;
                    }

                    updateModWaveFileName(file.getFileName());
                    ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                    ctx.audioProcessor.wtPlusModWavePath = file.getFullPathName();
                });
        }
    );
}

// 読み込んだ波形メモリファイルを 32 サンプルへ落として反映する
bool GuiWtPlus::applyModWaveFile(const juce::File& file, bool isWt2)
{
    juce::StringArray lines;
    file.readLines(lines);

    if (lines.size() == 0) return false;

    int sampleCount = lines[0].trim().getIntValue();

    if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
            "Invalid WT File", "Sample count must be 32, 64, 128, or 256.");
        return false;
    }

    std::vector<float> values(sampleCount, 0.0f);

    if (isWt2) {
        int resNumber = (lines.size() > 1) ? lines[1].trim().getIntValue() : 0;

        if (resNumber != 16 && resNumber != 32 && resNumber != 64 && resNumber != 128 && resNumber != 256) {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                "Invalid WT File", "Resolution must be 16, 32, 64, 128, or 256.");
            return false;
        }

        // .wt2 は 3 行目以降が 0〜(解像度-1) の整数。中央を 0 として -1.0〜1.0 に正規化する。
        float center = (float)(resNumber >> 1);

        for (int i = 0; i < sampleCount; ++i) {
            if (i + 2 < lines.size()) {
                int raw = std::clamp(lines[i + 2].getIntValue(), 0, resNumber - 1);
                values[i] = std::clamp(((float)raw - center) / center, -1.0f, 1.0f);
            }
        }
    }
    else {
        // .wt は 2 行目以降が -1.0〜1.0 の実数
        for (int i = 0; i < sampleCount; ++i) {
            if (i + 1 < lines.size()) {
                values[i] = std::clamp(lines[i + 1].getFloatValue(), -1.0f, 1.0f);
            }
        }
    }

    // 実機の波形メモリは 32 サンプルなので、そこへ落とす。
    //   Smooth ON  : 区間平均してから元のピークへ正規化する。
    //                32 点のナイキスト(16 次)より上の成分が低い次数へ
    //                フルの振幅で折り返すのを抑えつつ、変調の振れ幅は保つ。
    //   Smooth OFF : 単純間引き。元波形の値をそのまま拾う。
    const int step = sampleCount / 32;
    std::array<float, 32> reduced = { 0.0f };

    if (modWaveSmoothBtn.getToggleState() && step > 1) {
        float srcPeak = 0.0f;
        for (int i = 0; i < sampleCount; ++i) {
            srcPeak = std::max(srcPeak, std::fabs(values[i]));
        }

        float dstPeak = 0.0f;
        for (int i = 0; i < 32; ++i) {
            float sum = 0.0f;
            for (int k = 0; k < step; ++k) sum += values[i * step + k];

            reduced[i] = sum / (float)step;
            dstPeak = std::max(dstPeak, std::fabs(reduced[i]));
        }

        // 平均でなまったぶんのピークを戻す (無音の波形はそのまま)
        if (srcPeak > 1.0e-6f && dstPeak > 1.0e-6f) {
            float gain = srcPeak / dstPeak;

            for (int i = 0; i < 32; ++i) {
                reduced[i] = std::clamp(reduced[i] * gain, -1.0f, 1.0f);
            }
        }
    }
    else {
        for (int i = 0; i < 32; ++i) reduced[i] = values[i * step];
    }

    for (int i = 0; i < 32; ++i) {
        if (modWaveParams[i] != nullptr) {
            modWaveParams[i]->setValueNotifyingHost(modWaveParams[i]->convertTo0to1(reduced[i]));
        }
    }

    return true;
}

void GuiWtPlus::clearModWave()
{
    for (int i = 0; i < 32; ++i) {
        if (modWaveParams[i] != nullptr) {
            modWaveParams[i]->setValueNotifyingHost(modWaveParams[i]->convertTo0to1(0.0f));
        }
    }

    ctx.audioProcessor.wtPlusModWavePath = juce::String();

    updateModWaveFileName(Io::empty);
}

void GuiWtPlus::updateModWaveFileName(const juce::String& fileName)
{
    modWaveFileNameLabel.setText(fileName, juce::dontSendNotification);
}

// ==============================================================================
// チャンネルパラメータの入出力
// ==============================================================================
void GuiWtPlus::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::wtPlusParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                if (lines.size() == 0) return;

                int index = 0;

                // Level
                levelComponent.setImportingParams(lines, index);

                // Wave
                slotSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
                interpolateButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
                stepsSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);

                // Modulation
                modEnableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
                modDepthSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
                modSpeedSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
                modShapeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);

                // Components
                fixComponent.setImportingParams(lines, index);
                ampEnvComponent.setImportingParams(lines, index);
                pitchEnvComponent.setImportingParams(lines, index);
                ssgHwEnv.setImportingParams(lines, index);
                ssgSwEnvComponent.setImportingParams(lines, index);
                ssgSwEnv11Component.setImportingParams(lines, index);
                ssgSwPEnv11Component.setImportingParams(lines, index);
                mulDetuneComponent.setImportingParams(lines, index);
                lfoComponent.setImportingParams(lines, index);
                qualityComponent.setImportingParams(lines, index);
                unisonComponent.setImportingParams(lines, index);
            }
        });
}

void GuiWtPlus::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::wtPlusParam), Io::ExtensionGlob::wtPlusParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                // Level
                content += levelComponent.getExportedParams();

                // Wave
                content += juce::String(slotSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(interpolateButton.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(stepsSelector.getSelectedItemIndex()) + "\n";

                // Modulation
                content += juce::String(modEnableButton.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(modDepthSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(modSpeedSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(modShapeSelector.getSelectedItemIndex()) + "\n";

                // Components
                content += fixComponent.getExportedParams();
                content += ampEnvComponent.getExportedParams();
                content += pitchEnvComponent.getExportedParams();
                content += ssgHwEnv.getExportedParams();
                content += ssgSwEnvComponent.getExportedParams();
                content += ssgSwEnv11Component.getExportedParams();
                content += ssgSwPEnv11Component.getExportedParams();
                content += mulDetuneComponent.getExportedParams();
                content += lfoComponent.getExportedParams();
                content += qualityComponent.getExportedParams();
                content += unisonComponent.getExportedParams();

                file.replaceWithText(content);
            }
        });
}
