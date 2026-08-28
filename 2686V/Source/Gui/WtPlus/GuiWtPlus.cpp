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

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, WtPlusGuiText::Group::mainGroup);
    waveGroup.setup(*this, WtPlusGuiText::Group::waveGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 2K", 2000);

    // ==========================================================
    // WAVE
    // ==========================================================
    waveCat.setupHwCategory({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Category::wave, .enableChangeDetailVisible = true });

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
    slotsCat.setupHwCategory({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Category::slots, .enableChangeDetailVisible = true });

    for (int i = 0; i < Global::WtPlus::slots; ++i) {
        slotWtBtn[i].setup({ .parent = waveGroup.contentCanvas, .title = WtPlusGuiText::Wt::Slots::wt, .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true});
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
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, ctx.audioProcessor.wtPlusModWavePath);

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
    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = WtPlusGuiText::Category::util, .enableChangeDetailVisible = true });

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
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);
    ieSsgSwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW Env", ssgSwEnvComponent);
    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11Component);
    ieSsgSwPEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11Component);
    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

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

    modComponent.layoutComponent(mRect);

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

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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
    ieWtMod.setVisible(visible);
    ieSsgSwEnv.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
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
        ieWtMod.layoutComponent(rect);
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
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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

    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11Component.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11Component.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    else {
        ampEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
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
// チャンネルパラメータの入出力
// ==============================================================================
void GuiWtPlus::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 2) return;

                qualityComponent.setBit(lines[0].getIntValue());
                qualityComponent.setRate(lines[1].getIntValue());
            }
        });
}

void GuiWtPlus::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default.quality"), Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(qualityComponent.getBit()) + "\n";
                content += juce::String(qualityComponent.getRate()) + "\n";

                file.replaceWithText(content);
            }
        });
}

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
                modComponent.setImportingBaseParams(lines, index);
                modComponent.setImportingShapeParam(lines, index);

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
                content += modComponent.getExportedBaseParams();
                content += modComponent.getExportedShapeParam();

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
