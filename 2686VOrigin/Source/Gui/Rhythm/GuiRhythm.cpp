#include <vector>

#include "./GuiRhythm.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Rhythm/ProcessorRhythmKeys.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiRhythmValues.h"
#include "./GuiRhythmText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiRhythmHelpers.h"

#include "../../Core/Processor/PluginProcessorStateKey.h"

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

void RhythmPadGui::setup(juce::Component &parent, int index, juce::String padName, int& tabOrder)
{
    auto setupPanBtn = [this](juce::Component& parent, GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            parent.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    parent.addAndMakeVisible(this);

    juce::String padPrefix = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(index);
    juce::String padTitle = RhythmGuiText::Group::padPrefix +  " " + juce::String(index + 1) + " (" + padName + ")";

    // メイングループ
    mainGroup.setup(*this, padTitle);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::form, .detailVisible = true, .enableChangeDetailVisible = true });

    qualityPcmComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

    // 音声ファイルロードボタン
    loadButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::File::load, .isReset = false });
    loadButton.addListener(&ctx.editor);
    loadButton.setWantsKeyboardFocus(true);
    loadButton.setExplicitFocusOrder(++tabOrder);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = mainGroup.contentCanvas, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centred);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // パッド音声アンロード
    clearButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::File::clear, .textColor = juce::Colours::white, .isReset = false });
    clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this, index]
        {
            // 1. 特定のパッドをアンロード
            ctx.audioProcessor.unloadRhythmFile(index);

            // 2. ファイル名表示を更新
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    formSeparator.setupComponent(mainGroup.contentCanvas);

    optionalCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::optional, .enableChangeDetailVisible = true });

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::pcmOffset, .title = RhythmGuiText::Rhythm::Pad::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::pcmRatio, .title = RhythmGuiText::Rhythm::Pad::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::lpEnable, .title = RhythmGuiText::Rhythm::Pad::loopPointEnable, .isReset = true });
    loopPointEnableButton.setWantsKeyboardFocus(true);
    loopPointEnableButton.setExplicitFocusOrder(++tabOrder);

    loopPointStartSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::lpStart, .title = RhythmGuiText::Rhythm::Pad::loopPointStart, .isReset = true });
    loopPointStartSlider.setWantsKeyboardFocus(true);
    loopPointStartSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEndSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::lpEnd, .title = RhythmGuiText::Rhythm::Pad::loopPointEnd, .isReset = true });
    loopPointEndSlider.setWantsKeyboardFocus(true);
    loopPointEndSlider.setExplicitFocusOrder(++tabOrder);

    // Vol
    volSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::vol, .title = RhythmGuiText::Rhythm::Pad::vol, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::Tn::tone, .title = RhythmGuiText::Rhythm::Pad::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::Tn::noise, .title = RhythmGuiText::Rhythm::Pad::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::Tn::freq, .title = RhythmGuiText::Rhythm::Pad::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::Tn::mix , .title = RhythmGuiText::Rhythm::Pad::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Rhythm::Pad::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Rhythm::Pad::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Rhythm::Pad::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::oneShot, .title = RhythmGuiText::Rhythm::Pad::oneShot, .isReset = true });
    oneShotButton.setWantsKeyboardFocus(true);
    oneShotButton.setExplicitFocusOrder(++tabOrder);

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::note, .title = RhythmGuiText::Rhythm::Pad::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.setWantsKeyboardFocus(true);
    noteSlider.setExplicitFocusOrder(++tabOrder);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::pan, .enableChangeDetailVisible = true });

    // パンポット
    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + CPK::pan, .title = RhythmGuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);
    panSlider.setRange(0.0f, 1.0f);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = RhythmGuiText::Rhythm::Pad::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(0.0f, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = RhythmGuiText::Rhythm::Pad::Pan::c, .isReset = false });
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0.5f, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = RhythmGuiText::Rhythm::Pad::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1.0f, juce::sendNotification);
        };

    fixComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, "-> 440", 440);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, CPK::pitchAdsr + CPK::bypass, RhythmGuiText::Rhythm::Pad::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, CPK::ssgSwEnv + CPK::bypass, RhythmGuiText::Rhythm::Pad::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, RhythmGuiText::Rhythm::Pad::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, RhythmGuiText::Rhythm::Pad::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, ctx.audioProcessor.modWavePaths[padPrefix]);

    setupGraph();
    updateGraph();
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto ppadRect = content.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);

    ppadRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    // グラフ用の区画を確保
    layoutGraph(ppadRect);
    updateGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(ppadRect.translated(-content.getX(), -content.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> padRect(0, 0, mainGroup.getContentWidth(), 2000);

    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });

    layoutFormCat(padRect);

    layoutOptionalCat(padRect);

    layoutPanCat(padRect);

    ampEnvComponent.layoutComponent(padRect);
    modComponent.layoutComponent(padRect);

    ssgHwEnv.layoutComponent(padRect);

    ssgSwEnvComponent.layoutComponent(padRect);

    ssgSwEnv11Component.layoutComponent(padRect);

    pitchEnvComponent.layoutComponent(padRect);

    ssgSwPEnv11Component.layoutComponent(padRect);

    mulDetuneComponent.layoutComponent(padRect);

    lfoComponent.layoutComponent(padRect);

    fixComponent.layoutComponent(padRect);

    layoutQualityCat(padRect);

    int usedHeight = 2000 - padRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);
}

void RhythmPadGui::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

bool RhythmPadGui::isThis(juce::Button* button)
{
    return button == &loadButton;
}

void RhythmPadGui::updatePadVisible(bool visible) {
    mainGroup.setVisible(visible);
    fileNameLabel.setVisible(visible);
    loadButton.setVisible(visible);
    clearButton.setVisible(visible);
    formCat.setVisible(visible);
    optionalCat.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    loopPointEnableButton.setVisible(visible);
    loopPointStartSlider.setVisibleWithLabel(visible);
    loopPointEndSlider.setVisibleWithLabel(visible);
	qualityPcmComponent.setVisibles(visible);
    panCat.setVisible(visible);
    noteSlider.setVisibleWithLabel(visible);
    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);
    volSlider.setVisibleWithLabel(visible);
    toneSlider.setVisibleWithLabel(visible);
    noiseSlider.setVisibleWithLabel(visible);
    noiseFreqSlider.setVisibleWithLabel(visible);
    mixSlider.setVisibleWithLabel(visible);
    mixSetTone.setVisible(visible);
    mixSetMix.setVisible(visible);
    mixSetNoise.setVisible(visible);
    oneShotButton.setVisible(visible);
    fixComponent.setVisible(visible);
    ampEnvComponent.setVisible(visible);
    pitchEnvComponent.setVisible(visible);
    ssgSwEnvComponent.setVisible(visible);
    mulDetuneComponent.setVisible(visible);
    lfoComponent.setVisible(visible);
    graph.setVisible(visible);
    graphBtnAmp.setVisible(visible);
    graphBtnPitch.setVisible(visible);
    graphBtnSsg.setVisible(visible);
    graphBtnSsg11.setVisible(visible);
    graphBtnSsgP11.setVisible(visible);
    graphSeparator.setVisible(visible);
}

void RhythmPadGui::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    loadButton.setVisible(visible);
    fileNameLabel.setVisible(visible);
    clearButton.setVisible(visible);
    formSeparator.setVisible(visible);
    toneSlider.setVisibleWithLabel(visible);
    noiseSlider.setVisibleWithLabel(visible);
    noiseFreqSlider.setVisibleWithLabel(visible);
    mixSlider.setVisibleWithLabel(visible);
    mixSetTone.setVisible(visible);
    mixSetMix.setVisible(visible);
    mixSetNoise.setVisible(visible);

    if (visible)
    {
        layoutRowRhythmPadPcmFile({ .rect = rect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton });
        formSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void RhythmPadGui::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityPcmComponent.layoutComponent(rect);
}

void RhythmPadGui::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutRowThreeComps({
            .rect = rect,
            .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn
            });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void RhythmPadGui::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    oneShotButton.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    loopPointEnableButton.setVisible(visible);
    loopPointStartSlider.setVisibleWithLabel(visible);
    loopPointEndSlider.setVisibleWithLabel(visible);
    noteSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutRow({ .rowRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
        layoutRow({ .rowRect = rect, .component = &oneShotButton });
        layoutRow({ .rowRect = rect, .component = &loopPointEnableButton });
        layoutRow({ .rowRect = rect, .label = &loopPointStartSlider.label, .component = &loopPointStartSlider, });
        layoutRow({ .rowRect = rect, .label = &loopPointEndSlider.label, .component = &loopPointEndSlider, });
        layoutRow({ .rowRect = rect, .label = &noteSlider.label, .component = &noteSlider, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void RhythmPadGui::setupGraph()
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

void RhythmPadGui::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GraphMode::SsgSwP11, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void RhythmPadGui::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(RhythmGuiValue::Pad::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(RhythmGuiValue::Pad::Graph::ButtonHeight);
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
void RhythmPadGui::updateGraph()
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

void RhythmPadGui::copyParams(CopyRhythmPad& copyObj) {
    copyObj.base.level = volSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();
    copyObj.isOneShot = oneShotButton.getToggleState();
    copyObj.noteNumber = noteSlider.getValue();
    copyObj.pcm.pcmOffset = pcmOffsetSlider.getValue();
    copyObj.pcm.pcmRatio = pcmRatioSlider.getValue();
    copyObj.quality.mode = qualityPcmComponent.getMode();
    copyObj.quality.rate = qualityPcmComponent.getRate();
    copyObj.toneLevel = toneSlider.getValue();
    copyObj.noiseLevel = noiseSlider.getValue();
    copyObj.noiseFreq = noiseFreqSlider.getValue();
    copyObj.mix = mixSlider.getValue();

    ampEnvComponent.copyParams(copyObj.aAdsr);
    pitchEnvComponent.copyParams(copyObj.pAdsr);
}

void RhythmPadGui::pasteParams(CopyRhythmPad& copyObj) {
    volSlider.setValue(copyObj.base.level, juce::sendNotification);
    panSlider.setValue(copyObj.pan.pan, juce::sendNotification);
    oneShotButton.setToggleState(copyObj.isOneShot, juce::sendNotification);
    noteSlider.setValue(copyObj.noteNumber, juce::sendNotification);
    pcmOffsetSlider.setValue(copyObj.pcm.pcmOffset, juce::sendNotification);
    pcmRatioSlider.setValue(copyObj.pcm.pcmRatio, juce::sendNotification);
    qualityPcmComponent.setMode(copyObj.quality.mode);
    qualityPcmComponent.setRate(copyObj.quality.rate);
    toneSlider.setValue(copyObj.toneLevel);
    noiseSlider.setValue(copyObj.noiseLevel);
    noiseFreqSlider.setValue(copyObj.noiseFreq);
    mixSlider.setValue(copyObj.mix);

    ampEnvComponent.pasteParams(copyObj.aAdsr);
    pitchEnvComponent.pasteParams(copyObj.pAdsr);
}

void RhythmPadGui::importToneNoiseParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultToneNoiseParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importToneNoiseParamFile, defaultDir, Io::ExtensionGlob::ToneNoiseParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 4) return;

                toneSlider.setValue(lines[0].getFloatValue(), juce::sendNotification);
                noiseSlider.setValue(lines[1].getFloatValue(), juce::sendNotification);
                noiseFreqSlider.setValue(lines[2].getFloatValue(), juce::sendNotification);
                mixSlider.setValue(lines[3].getFloatValue(), juce::sendNotification);
            }
        });
}

void RhythmPadGui::exportToneNoiseParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultToneNoiseParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportToneNoiseParamFile, defaultDir.getChildFile("default.toneNoise"), Io::ExtensionGlob::ToneNoiseParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(toneSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(noiseSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(noiseFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(mixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                file.replaceWithText(content);
            }
        });
}

void RhythmPadGui::importLfoParam() {
    lfoComponent.importParams();
}

void RhythmPadGui::exportLfoParam() {
    lfoComponent.exportParams();
}

void RhythmPadGui::importAmpEnvParam() {
    ampEnvComponent.importParams();
}

void RhythmPadGui::exportAmpEnvParam() {
    ampEnvComponent.exportParams();
}

void RhythmPadGui::importPitchEnvParam() {
    pitchEnvComponent.importParams();
}

void RhythmPadGui::exportPitchEnvParam() {
    pitchEnvComponent.exportParams();
}

void RhythmPadGui::importWtModParam() { modComponent.importParams(); }

void RhythmPadGui::exportWtModParam() { modComponent.exportParams(); }

void RhythmPadGui::importSsgHwEnvParam() {
    ssgHwEnv.importParams();
}

void RhythmPadGui::exportSsgHwEnvParam() {
    ssgHwEnv.exportParams();
}

void RhythmPadGui::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void RhythmPadGui::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void RhythmPadGui::importSsgSwEnv11Param() {
    ssgSwEnv11Component.importParams();
}

void RhythmPadGui::exportSsgSwEnv11Param() {
    ssgSwEnv11Component.exportParams();
}

void RhythmPadGui::importSsgSwPEnv11Param() {
    ssgSwPEnv11Component.importParams();
}

void RhythmPadGui::exportSsgSwPEnv11Param() {
    ssgSwPEnv11Component.exportParams();
}

void RhythmPadGui::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void RhythmPadGui::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void RhythmPadGui::importPcmPlayParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::PcmQualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 3) return;

                qualityPcmComponent.setMode(lines[0].getIntValue());
                qualityPcmComponent.setRate(lines[1].getIntValue());
                qualityPcmComponent.setInterp(lines[2].getIntValue());
            }
        });
}

void RhythmPadGui::exportPcmPlayParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default.pcmQuality"), Io::ExtensionGlob::PcmQualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(qualityPcmComponent.getMode()) + "\n";
                content += juce::String(qualityPcmComponent.getRate()) + "\n";
                content += juce::String(qualityPcmComponent.getInterp()) + "\n";

                file.replaceWithText(content);
            }
        });
}

void RhythmPadGui::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::PcmQualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 3) return;

                qualityPcmComponent.setMode(lines[0].getIntValue());
                qualityPcmComponent.setRate(lines[1].getIntValue());
                qualityPcmComponent.setInterp(lines[2].getIntValue());
            }
        });
}

void RhythmPadGui::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default.pcmQuality"), Io::ExtensionGlob::PcmQualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(qualityPcmComponent.getMode()) + "\n";
                content += juce::String(qualityPcmComponent.getRate()) + "\n";
                content += juce::String(qualityPcmComponent.getInterp()) + "\n";

                file.replaceWithText(content);
            }
        });
}

void RhythmPadGui::setImportingParams(int p, juce::StringArray& lines, int& index) {
    // linesの終端を超えていたら（Originなどの6パッドなど、パッド数が8未満の場合）、初期値でリセットして処理を抜ける
    if (index >= lines.size()) {
        ctx.audioProcessor.unloadRhythmFile(p);
        fileNameLabel.setText(Io::empty, juce::dontSendNotification);

        // 基本パラメータ初期値
        volSlider.setValue(0.0, juce::sendNotification);
        panSlider.setValue(0.5, juce::sendNotification);
        oneShotButton.setToggleState(false, juce::sendNotification);
        noteSlider.setValue(60, juce::sendNotification);

        // PCM Play
        pcmOffsetSlider.setValue(0.0, juce::sendNotification);
        pcmRatioSlider.setValue(1.0, juce::sendNotification);
        loopPointEnableButton.setToggleState(false, juce::sendNotification);
        loopPointStartSlider.setValue(0.0, juce::sendNotification);
        loopPointEndSlider.setValue(1.0, juce::sendNotification);

        // Tone/Noise
        toneSlider.setValue(1.0, juce::sendNotification);
        noiseSlider.setValue(0.0, juce::sendNotification);
        noiseFreqSlider.setValue(0.0, juce::sendNotification);
        mixSlider.setValue(0.0, juce::sendNotification);

        juce::StringArray emptyLines;
        int dummyIndex = 0;
        fixComponent.setImportingParams(emptyLines, dummyIndex);
        ampEnvComponent.setImportingParams(emptyLines, dummyIndex);
        pitchEnvComponent.setImportingParams(emptyLines, dummyIndex);
        ssgHwEnv.setImportingParams(emptyLines, dummyIndex);
        ssgSwEnvComponent.setImportingParams(emptyLines, dummyIndex);
        ssgSwEnv11Component.setImportingParams(emptyLines, dummyIndex);
        ssgSwPEnv11Component.setImportingParams(emptyLines, dummyIndex);
        mulDetuneComponent.setImportingParams(emptyLines, dummyIndex);
        lfoComponent.setImportingParams(emptyLines, dummyIndex);
        qualityPcmComponent.setImportingParams(emptyLines, dummyIndex);

        return;
    }

    // Form
    if (fileNameLabel.getText() != lines[index]) {
        ctx.audioProcessor.unloadRhythmFile(p);
    }

    fileNameLabel.setText(lines[index++], juce::dontSendNotification);

    if (fileNameLabel.getText().isNotEmpty()) {
        ctx.audioProcessor.loadRhythmFile(fileNameLabel.getText(), p);
    }

    volSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    oneShotButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    noteSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

    // Tone/Noise
    toneSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    noiseSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    noiseFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    mixSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // PCM Play
    pcmOffsetSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pcmRatioSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    loopPointEnableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopPointStartSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    loopPointEndSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

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
    qualityPcmComponent.setImportingParams(lines, index);
}

juce::String RhythmPadGui::getExportedParams() {
    juce::String content = "";

    // Form
    content += fileNameLabel.getText() + "\n";

    // Optional
    content += juce::String(volSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(panSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(oneShotButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(noteSlider.getValue()) + "\n";

    // Tone/Noise
    content += juce::String(toneSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(noiseSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(noiseFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(mixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

    // PCM Play
    content += juce::String(pcmOffsetSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(pcmRatioSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(loopPointEnableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(loopPointStartSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(loopPointEndSlider.getValue(), Global::floatDecimalPlaces) + "\n";

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
    content += qualityPcmComponent.getExportedParams();

    return content;
}

GuiRhythm::GuiRhythm(const GuiContext& context) :
    GuiBase(context),
    mainGroup(context),
    presetName(context),
    viewModeComp(context),
    levelComponent(context),
    unisonComponent(context),
    midiComponent(context),
    utilityCat(context),
    broadcastLevelButton(context),
    uSep001(context),
    copyPadParamBtn(context),
    copyPadFromSlider(context),
    copyPadToSlider(context),
    uSep002(context),
    ieToneNoise(context),
    ieLfo(context),
    ieAmpEnv(context),
    iePitchEnv(context),
    ieSsgHwEnv(context),
    ieWtMod(context),
    ieSsgSwEnv(context),
    ieSsgSwEnv11(context),
    ieSsgSwPEnv11(context),
    ieDetune(context),
    ieQuality(context),
    iePcmPlay(context),
	ieChPadParam(context),
    targerPadSlider(context),
    uSep003(context),
    ieUnison(context),
	ieChParam(context),
    pads{ { {context}, {context}, {context}, {context}, {context}, {context} } }
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    
    int mode = context.audioProcessor.apvts.state.getProperty(ProcessorStateKey::rhythmViewMode, (int)GuiComponentViewModes::Twin);
    viewMode = (GuiComponentViewModes)mode;
}


void GuiRhythm::setup()
{
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;

    // パッド名定義
    const std::array<juce::String, RhythmPrValue::pads> padNames = { "BD", "SD", "RIM", "TOM", "CYMBAL", "HI-HAT" };

    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    copyPadParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Pad Params", .bgColor = juce::Colours::turquoise.darker(0.3f) });
    copyPadParamBtn.setWantsKeyboardFocus(true);
    copyPadParamBtn.setExplicitFocusOrder(++tabOrder);
    copyPadParamBtn.onClick = [this] {
        int from = copyPadFromSlider.getValue() - 1;
        int to = copyPadToSlider.getValue() - 1;

        ctx.editor.copyRhythmPadParams(from, to);
        };

    copyPadFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyPadFromSlider.setRange(1.0, 8.0, 1.0);
    copyPadFromSlider.setNumDecimalPlacesToDisplay(0);
    copyPadFromSlider.setValue(1, juce::sendNotification);
    copyPadFromSlider.setWantsKeyboardFocus(true);
    copyPadFromSlider.setExplicitFocusOrder(++tabOrder);
    copyPadFromSlider.onValueChange = [this] {
        int from = copyPadFromSlider.getValue() - 1;
        int to = copyPadToSlider.getValue() - 1;

        copyPadParamBtn.setEnabled(from != to);
        };

    copyPadToSlider.setup({ .parent = mainGroup.contentCanvas, .title = "TO", .isReset = false });
    copyPadToSlider.setRange(1.0, 8.0, 1.0);
    copyPadToSlider.setNumDecimalPlacesToDisplay(0);
    copyPadToSlider.setValue(2, juce::sendNotification);
    copyPadToSlider.setWantsKeyboardFocus(true);
    copyPadToSlider.setExplicitFocusOrder(++tabOrder);
    copyPadToSlider.onValueChange = [this] {
        int from = copyPadFromSlider.getValue() - 1;
        int to = copyPadToSlider.getValue() - 1;

        copyPadParamBtn.setEnabled(from != to);
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    ieToneNoise.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Tone/Noise");
    ieToneNoise.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importToneNoiseParam(padIndex); };
    ieToneNoise.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportToneNoiseParam(padIndex); };

    ieLfo.setupComponentOp(mainGroup.contentCanvas, tabOrder, "LFO");
    ieLfo.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importLfoParam(padIndex); };
    ieLfo.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportLfoParam(padIndex); };

    ieDetune.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Detune");
    ieDetune.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importDetuneParam(padIndex); };
    ieDetune.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportDetuneParam(padIndex); };

    ieAmpEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Amp Env");
    ieAmpEnv.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importAmpEnvParam(padIndex); };
    ieAmpEnv.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportAmpEnvParam(padIndex); };

    iePitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    iePitchEnv.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importPitchEnvParam(padIndex); };
    iePitchEnv.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportPitchEnvParam(padIndex); };

    ieSsgHwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW Env");
    ieSsgHwEnv.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; pads[padIndex].importSsgHwEnvParam(); };
    ieSsgHwEnv.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; pads[padIndex].exportSsgHwEnvParam(); };

    ieWtMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Modulation");
    ieWtMod.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; pads[padIndex].importWtModParam(); };
    ieWtMod.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; pads[padIndex].exportWtModParam(); };

    ieSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieSsgSwEnv.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importSsgSwEnvParam(padIndex); };
    ieSsgSwEnv.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportSsgSwEnvParam(padIndex); };

    ieSsgSwEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieSsgSwEnv11.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importSsgSwEnv11Param(padIndex); };
    ieSsgSwEnv11.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportSsgSwEnv11Param(padIndex); };

    ieSsgSwPEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieSsgSwPEnv11.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importSsgSwPEnv11Param(padIndex); };
    ieSsgSwPEnv11.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportSsgSwPEnv11Param(padIndex); };

    ieQuality.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importQualityParam(padIndex); };
    ieQuality.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportQualityParam(padIndex); };

    iePcmPlay.setupComponentOp(mainGroup.contentCanvas, tabOrder, "PCM Play");
    iePcmPlay.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importPcmPlayParam(padIndex); };
    iePcmPlay.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportPcmPlayParam(padIndex); };

    ieChPadParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pad Params");
    ieChPadParam.onClickImport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; importPadChParam(padIndex); };
    ieChPadParam.onClickExport = [this] { int padIndex = (int)targerPadSlider.getValue() - 1; exportPadChParam(padIndex); };

    targerPadSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Pad", .isReset = false });
    targerPadSlider.setRange(1.0, 8.0, 1.0);
    targerPadSlider.setNumDecimalPlacesToDisplay(0);
    targerPadSlider.setValue(1, juce::sendNotification);
    targerPadSlider.setWantsKeyboardFocus(true);
    targerPadSlider.setExplicitFocusOrder(++tabOrder);

    uSep003.setupComponent(mainGroup.contentCanvas);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    viewModeComp.setupComponent(*this, tabOrder);
    viewModeComp.onChangeViewMode = [this] (GuiComponentViewModes mode) {
        viewMode = mode;
        ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::rhythmViewMode, (int)viewMode, nullptr);
        ctx.editor.resized();
        };

    // Setup Pads
    for (int i = 0; i < RhythmPrValue::pads; ++i)
    {
        pads[i].setup(*this, i, padNames[i], tabOrder);
    }

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);
}

void GuiRhythm::layout(juce::Rectangle<int> content)
{
    // Top section for Master Volume
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(RhythmGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    viewModeComp.layoutComponent(mmRect);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

    levelComponent.layoutComponent(mRect);
    
    unisonComponent.layoutComponent(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    int pWidth = pageArea.getWidth() / 4;

    switch (viewMode) {
    case GuiComponentViewModes::Top:
        {
            for (int i = 4; i < RhythmPrValue::pads; i++) {
                updatePadVisible(i, false);
            }

            for (int i = 0; i < 4; i++) {
                updatePadVisible(i, true);

                auto padArea = pageArea.removeFromLeft(pWidth);

                layoutPad(i, padArea);
            }

            break;
        }
    case GuiComponentViewModes::Bottom:
        {
            for (int i = 0; i < 4; i++) {
                updatePadVisible(i, false);
            }

            for (int i = 4; i < RhythmPrValue::pads; i++) {
                updatePadVisible(i, true);

                auto padArea = pageArea.removeFromLeft(pWidth);

                layoutPad(i, padArea);
            }

            break;
        }
    case GuiComponentViewModes::Twin:
        {
            auto topPadsArea = pageArea.removeFromTop(RhythmGuiValue::Pad::height);
            auto bottomPadsArea = pageArea.removeFromTop(RhythmGuiValue::Pad::height);
        
            for (int i = 0; i < 4; i++) {
                updatePadVisible(i, true);

                auto padArea = topPadsArea.removeFromLeft(pWidth);

                layoutPad(i, padArea);
            }

            for (int i = 4; i < RhythmPrValue::pads; i++) {
                updatePadVisible(i, true);

                auto padArea = bottomPadsArea.removeFromLeft(pWidth);

                layoutPad(i, padArea);
            }

            break;
        }
    }
}


void GuiRhythm::layoutPad(int padIndex, juce::Rectangle<int>& rect) {
    pads[padIndex].setBounds(rect);
    pads[padIndex].layout(pads[padIndex].getLocalBounds());
}

void GuiRhythm::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyPadParamBtn.setVisible(visible);
    copyPadFromSlider.setVisibleWithLabel(visible);
    copyPadToSlider.setVisibleWithLabel(visible);
    uSep002.setVisible(visible);
    ieToneNoise.setVisible(visible);
    ieLfo.setVisible(visible);
    ieDetune.setVisible(visible);
    ieAmpEnv.setVisible(visible);
    iePitchEnv.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieWtMod.setVisible(visible);
    ieSsgSwEnv.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieChPadParam.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    iePcmPlay.setVisible(visible);
    ieChParam.setVisible(visible);
    targerPadSlider.setVisibleWithLabel(visible);
    uSep003.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyPadParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyPadFromSlider.label, .component = &copyPadFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyPadToSlider.label, .component = &copyPadToSlider });

        uSep002.layoutComponent(rect);

        ieToneNoise.layoutComponent(rect);
        rect.removeFromTop(4);
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
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        iePcmPlay.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChPadParam.layoutComponent(rect);
        rect.removeFromTop(4);
        layoutMain({ .mainRect = rect, .label = &targerPadSlider.label, .component = &targerPadSlider });

        uSep003.layoutComponent(rect);

        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiRhythm::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    for (int i = 0; i < RhythmPrValue::pads; ++i)
    {
        pads[i].removeLoadButtonListener(editor);
    }
}

void GuiRhythm::buttonClicked(juce::Button* button, juce::AudioFormatManager& formatManager, std::unique_ptr<juce::FileChooser>& fileChooser)
{
    for (int i = 0; i < RhythmPrValue::pads; ++i)
    {
        auto& pad = pads[i];

        if (pad.isThis(button))
        {
            auto fileFilter = formatManager.getWildcardForAllFormats();
            auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

            // Capture index 'i' by value
            fileChooser->launchAsync(folderChooserFlags, [this, i](const juce::FileChooser& fc)
                {
                    auto file = fc.getResult();
                    if (file.existsAsFile())
                    {
                        pads[i].updatePadFileName("Loading...");

                        juce::Timer::callAfterDelay(50, [this, i, file]()
                            {
                                // Load to specific pad index
                                ctx.audioProcessor.loadRhythmFile(file, i);

                                // Update label
                                pads[i].updatePadFileName(file.getFileName());

                                ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                            });
                    }
                });

            return; // Found
        }
    }

}

void GuiRhythm::updatePadFileName(int padIndex, const juce::String& fileName)
{
    pads[padIndex].updatePadFileName(fileName);
}

bool GuiRhythm::isThis(int index, juce::Button* button)
{
    return pads[index].isThis(button);
}

void GuiRhythm::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiRhythm::updatePadVisible(int idx, bool visible) {
    pads[idx].updatePadVisible(visible);
}

void GuiRhythm::initParams()
{
    this->ctx.audioProcessor.initParams("RHYTHM_");
    for (int i = 0; i < RhythmPrValue::pads; i++)
    {
        this->ctx.audioProcessor.unloadRhythmFile(i);
        updatePadFileName(i, Io::empty);
    }
}

void GuiRhythm::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiRhythm::copyPadParams(int p, CopyRhythmPad& copyObj) {
    pads[p].copyParams(copyObj);
}

void GuiRhythm::pastePadParams(int p, CopyRhythmPad& copyObj) {
    pads[p].pasteParams(copyObj);
}

void GuiRhythm::importToneNoiseParam(int p) {
    pads[p].importToneNoiseParam();
}

void GuiRhythm::exportToneNoiseParam(int p) {
    pads[p].exportToneNoiseParam();
}

void GuiRhythm::importLfoParam(int p) {
    pads[p].importLfoParam();
}

void GuiRhythm::exportLfoParam(int p) {
    pads[p].exportLfoParam();
}

void GuiRhythm::importAmpEnvParam(int p) {
    pads[p].importAmpEnvParam();
}

void GuiRhythm::exportAmpEnvParam(int p) {
    pads[p].exportAmpEnvParam();
}

void GuiRhythm::importPitchEnvParam(int p) {
    pads[p].importPitchEnvParam();
}

void GuiRhythm::exportPitchEnvParam(int p) {
    pads[p].exportPitchEnvParam();
}

void GuiRhythm::importSsgSwEnvParam(int p) {
    pads[p].importSsgSwEnvParam();
}

void GuiRhythm::exportSsgSwEnvParam(int p) {
    pads[p].exportSsgSwEnvParam();
}

void GuiRhythm::importDetuneParam(int p) {
    pads[p].importDetuneParam();
}

void GuiRhythm::exportDetuneParam(int p) {
    pads[p].exportDetuneParam();
}

void GuiRhythm::importQualityParam(int p) {
    pads[p].importQualityParam();
}

void GuiRhythm::exportQualityParam(int p) {
    pads[p].exportQualityParam();
}

void GuiRhythm::importPcmPlayParam(int p) {
    pads[p].importPcmPlayParam();
}

void GuiRhythm::exportPcmPlayParam(int p) {
    pads[p].exportPcmPlayParam();
}

void GuiRhythm::importSsgSwEnv11Param(int p) {
    pads[p].importSsgSwEnv11Param();
}

void GuiRhythm::exportSsgSwEnv11Param(int p) {
    pads[p].exportSsgSwEnv11Param();
}

void GuiRhythm::importSsgSwPEnv11Param(int p) {
    pads[p].importSsgSwPEnv11Param();
}

void GuiRhythm::exportSsgSwPEnv11Param(int p) {
    pads[p].importSsgSwPEnv11Param();
}

void GuiRhythm::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::rhythmParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                // Level
                levelComponent.setImportingParams(lines, index);

                // Components
                unisonComponent.setImportingParams(lines, index);

                for (int i = 0; i < RhythmPrValue::pads; i++) {
                    getImportingPadParams(i, lines, index);
                }
            }
        });

}

void GuiRhythm::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::rhythmParam), Io::ExtensionGlob::rhythmParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                // Level
                content += levelComponent.getExportedParams();

                // Components
                content += unisonComponent.getExportedParams();

                for (int i = 0; i < RhythmPrValue::pads; i++) {
                    content += setExportedPadParams(i);
                }

                file.replaceWithText(content);
            }
        });

}

void GuiRhythm::importPadChParam(int p) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::rhythmPadParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, p](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                getImportingPadParams(p, lines, index);
            }
        });

}

void GuiRhythm::exportPadChParam(int p) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::rhythmPadParam), Io::ExtensionGlob::rhythmPadParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, p](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += setExportedPadParams(p);

                file.replaceWithText(content);
            }
        });

}

void GuiRhythm::getImportingPadParams(int p, juce::StringArray& lines, int& index) {
    pads[p].setImportingParams(p, lines, index);
}

juce::String GuiRhythm::setExportedPadParams(int p) {
    return pads[p].getExportedParams();
}
