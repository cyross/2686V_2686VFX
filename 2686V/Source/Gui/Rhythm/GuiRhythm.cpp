#include <vector>

#include "./GuiRhythm.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Rhythm/ProcessorRhythmKeys.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiRhythmValues.h"
#include "./GuiRhythmText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiRhythmHelpers.h"

// 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM, 13: 4bit ADPCM, 14: 1bit DPCM
static std::vector<SelectItem> qualityItems = {
    {.name = " 1: Raw (32bit)", .value = 1 },
    {.name = " 2: 24-bit PCM",  .value = 2 },
    {.name = " 3: 20-bit PCM",  .value = 3 },
    {.name = " 4: 16-bit PCM",  .value = 4 },
    {.name = " 5: 12-bit PCM",  .value = 5 },
    {.name = " 6: 10-bit PCM",  .value = 6 },
    {.name = " 7: 9-bit PCM",   .value = 7 },
    {.name = " 8: 8-bit PCM",   .value = 8 },
    {.name = " 9: 7-bit PCM",   .value = 9 },
    {.name = "10: 6-bit PCM",   .value = 10 },
    {.name = "11: 5-bit PCM",   .value = 11 },
    {.name = "12: 4-bit PCM",   .value = 12 },
    {.name = "13: 4-bit ADPCM", .value = 13 },
    {.name = "14: 1-bit DPCM",  .value = 14 },
};

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 33.08kHz", .value = 6 },
    {.name = " 7: 32kHz",    .value = 7 },
    {.name = " 8: 22.05kHz", .value = 8 },
    {.name = " 9: 16kHz",    .value = 9 },
    {.name = "10: 12kHz",    .value = 10 },
    {.name = "11: 11kHz",    .value = 11 },
    {.name = "12: 8kHz",     .value = 12 },
    {.name = "12: 5.5kHz",   .value = 13 },
    {.name = "13: 4kHz",     .value = 14 },
    {.name = "15: 2kHz",     .value = 15 },
};

static std::vector<SelectItem> interpItems = {
    {.name = juce::String("") + "1: 補完なし (Nearest)", .value = 1 },
    {.name = juce::String("") + "2: 線形補間 (Linear)", .value = 2 },
    {.name = juce::String("") + "3: ガウス補完 (Gaussian)", .value = 3 }
};

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

void RhythmPadGui::setup(juce::Component &parent, int index, juce::String padName, int& tabOrder)
{
    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
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

    qualityCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::visibleQuality, .invisibleTitle = RhythmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    modeSelector.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::mode, .title = RhythmGuiText::Rhythm::Pad::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::rate, .title = RhythmGuiText::Rhythm::Pad::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    interpSelector.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::interp, .title = RhythmGuiText::Rhythm::Pad::interp, .items = interpItems, .isReset = true });
    interpSelector.setWantsKeyboardFocus(true);
    interpSelector.setExplicitFocusOrder(++tabOrder);

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
    clearButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::File::clear, .isReset = false });
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

    optionalCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::visibleOptional, .invisibleTitle = RhythmGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::pcmOffset, .title = RhythmGuiText::Rhythm::Pad::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::pcmRatio, .title = RhythmGuiText::Rhythm::Pad::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // Vol
    volSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::volume, .title = RhythmGuiText::Rhythm::Pad::vol, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::tone, .title = RhythmGuiText::Rhythm::Pad::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::noise, .title = RhythmGuiText::Rhythm::Pad::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::noiseFreq, .title = RhythmGuiText::Rhythm::Pad::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::mix , .title = RhythmGuiText::Rhythm::Pad::mix, .isReset = true });
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
    oneShotButton.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::oneShot, .title = RhythmGuiText::Rhythm::Pad::oneShot, .isReset = true });
    oneShotButton.setWantsKeyboardFocus(true);
    oneShotButton.setExplicitFocusOrder(++tabOrder);

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::note, .title = RhythmGuiText::Rhythm::Pad::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.setWantsKeyboardFocus(true);
    noteSlider.setExplicitFocusOrder(++tabOrder);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::visiblePan, .invisibleTitle = RhythmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    // パンポット
    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::pan, .title = RhythmGuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);
    panSlider.setRange(0.0f, 1.0f);

    setupPanBtn(panToLBtn, RhythmGuiText::Rhythm::Pad::Pan::l, tabOrder);
    setupPanBtn(panToCBtn, RhythmGuiText::Rhythm::Pad::Pan::c, tabOrder);
    setupPanBtn(panToRBtn, RhythmGuiText::Rhythm::Pad::Pan::r, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, "-> 440", 440);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, RhythmPrKey::pitchAdsr + RhythmPrKey::bypass, RhythmGuiText::Rhythm::Pad::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder, RhythmPrKey::ssgSwEnv + RhythmPrKey::bypass, RhythmGuiText::Rhythm::Pad::SsgSwEnv::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, padPrefix, tabOrder);

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
    juce::Rectangle<int> padRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutRowRhythmPadPcmFile({ .rect = padRect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton });

    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });
    layoutMain({ .mainRect = padRect, .label = &toneSlider.label, .component = &toneSlider, });
    layoutMain({ .mainRect = padRect, .label = &noiseSlider.label, .component = &noiseSlider });
    layoutMain({ .mainRect = padRect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
    layoutMain({ .mainRect = padRect, .label = &mixSlider.label, .component = &mixSlider });
    layoutMainThreeComps({ .rect = padRect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });

    layoutOptionalCat(padRect);

    layoutPanCat(padRect);

    fixComponent.layoutComponent(padRect);

    ampEnvComponent.layoutComponent(padRect);

    pitchEnvComponent.layoutComponent(padRect);

    ssgSwEnvComponent.layoutComponent(padRect);

    mulDetuneComponent.layoutComponent(padRect);

    lfoComponent.layoutComponent(padRect);

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

void RhythmPadGui::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .label = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);
    interpSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutRow({ .rowRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutRow({ .rowRect = rect, .label = &interpSelector.label, .component = &interpSelector, });
    }
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
            .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn,
            .compWidth = RhythmGuiValue::ParamGroup::RhythmPan::width,
            .compPaddingRight = RhythmGuiValue::ParamGroup::RhythmPan::paddingRight
            });
    }
}

void RhythmPadGui::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    oneShotButton.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    noteSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutRow({ .rowRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
        layoutRow({ .rowRect = rect, .component = &oneShotButton });
        layoutRow({ .rowRect = rect, .label = &noteSlider.label, .component = &noteSlider, });
    }
}

void RhythmPadGui::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    auto repaintGraph = [this]() { updateGraph(); };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator);
    graphSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void RhythmPadGui::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void RhythmPadGui::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(RhythmGuiValue::Pad::Graph::height + RhythmGuiValue::Pad::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(RhythmGuiValue::Pad::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(RhythmGuiValue::Pad::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 2;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void RhythmPadGui::updateGraph()
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
    else {
        ampEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
}

void RhythmPadGui::copyParams(CopyRhythmPad& copyObj) {
    copyObj.base.level = volSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();
    copyObj.isOneShot = oneShotButton.getToggleState();
    copyObj.noteNumber = noteSlider.getValue();
    copyObj.pcm.pcmOffset = pcmOffsetSlider.getValue();
    copyObj.pcm.pcmRatio = pcmRatioSlider.getValue();
    copyObj.quality.mode = modeSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();

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
    modeSelector.setSelectedId(copyObj.quality.mode);
    rateSelector.setSelectedId(copyObj.quality.rate);

    ampEnvComponent.pasteParams(copyObj.aAdsr);
    pitchEnvComponent.pasteParams(copyObj.pAdsr);
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

void RhythmPadGui::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void RhythmPadGui::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void RhythmPadGui::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void RhythmPadGui::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void GuiRhythm::setup()
{
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;

    // パッド名定義
    const std::array<juce::String, RhythmPrValue::pads> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + RhythmPrKey::level, .title = RhythmGuiText::Rhythm::vol, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::visibleUtil, .invisibleTitle = RhythmGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    mainGroup.contentCanvas.addAndMakeVisible(uSep001);
    uSep001.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

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

    mainGroup.contentCanvas.addAndMakeVisible(uSep002);
    uSep002.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::lfoFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importLfoParamButton.setWantsKeyboardFocus(true);
    importLfoParamButton.setExplicitFocusOrder(++tabOrder);
    importLfoParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importLfoParam(padIndex);
        };

    exportLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::lfoFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportLfoParamButton.setWantsKeyboardFocus(true);
    exportLfoParamButton.setExplicitFocusOrder(++tabOrder);
    exportLfoParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportLfoParam(padIndex);
        };

    importAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::ampEnvFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importAmpEnvParamButton.setWantsKeyboardFocus(true);
    importAmpEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importAmpEnvParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importAmpEnvParam(padIndex);
        };

    exportAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::ampEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportAmpEnvParamButton.setWantsKeyboardFocus(true);
    exportAmpEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportAmpEnvParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportAmpEnvParam(padIndex);
        };

    importPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::pitchEnvFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importPitchEnvParamButton.setWantsKeyboardFocus(true);
    importPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importPitchEnvParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importPitchEnvParam(padIndex);
        };

    exportPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::pitchEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportPitchEnvParamButton.setWantsKeyboardFocus(true);
    exportPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportPitchEnvParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportPitchEnvParam(padIndex);
        };

    importSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::ssgSwEnvFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    importSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importSsgSwEnvParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importSsgSwEnvParam(padIndex);
        };

    exportSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::ssgSwEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    exportSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportSsgSwEnvParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportSsgSwEnvParam(padIndex);
        };

    importDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::detuneFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importDetuneParamButton.setWantsKeyboardFocus(true);
    importDetuneParamButton.setExplicitFocusOrder(++tabOrder);
    importDetuneParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importDetuneParam(padIndex);
        };

    exportDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::detuneFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportDetuneParamButton.setWantsKeyboardFocus(true);
    exportDetuneParamButton.setExplicitFocusOrder(++tabOrder);
    exportDetuneParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportDetuneParam(padIndex);
        };

    targerPadSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Pad", .isReset = false });
    targerPadSlider.setRange(1.0, 8.0, 1.0);
    targerPadSlider.setNumDecimalPlacesToDisplay(0);
    targerPadSlider.setValue(1, juce::sendNotification);
    targerPadSlider.setWantsKeyboardFocus(true);
    targerPadSlider.setExplicitFocusOrder(++tabOrder);

    mainGroup.contentCanvas.addAndMakeVisible(uSep003);
    uSep003.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::unisonFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importUnisonParamButton.setWantsKeyboardFocus(true);
    importUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    importUnisonParamButton.onClick = [this] {
        importUnisonParam();
        };

    exportUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::unisonFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportUnisonParamButton.setWantsKeyboardFocus(true);
    exportUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    exportUnisonParamButton.onClick = [this] {
        exportUnisonParam();
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
    auto applyPads = [&](juce::Rectangle<int>& area, int width, int start, int length)
    {
        for (int i = start; i < start + length; ++i)
        {
            auto padArea = area.removeFromLeft(width);

            pads[i].setBounds(padArea);
            pads[i].layout(pads[i].getLocalBounds());
        }
    };

    // Top section for Master Volume
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(RhythmGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mmRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mmRect.removeFromTop(RhythmGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    unisonComponent.layoutComponent(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    auto topPadsArea = pageArea.removeFromTop(RhythmGuiValue::Pad::height);
    auto bottomPadsArea = pageArea.removeFromTop(RhythmGuiValue::Pad::height);

    // Remaining area for 8 pads
    applyPads(topPadsArea, topPadsArea.getWidth() / 4, 0, 4);
    applyPads(bottomPadsArea, bottomPadsArea.getWidth() / 4, 4, 4);
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
    importLfoParamButton.setVisible(visible);
    exportLfoParamButton.setVisible(visible);
    importAmpEnvParamButton.setVisible(visible);
    exportAmpEnvParamButton.setVisible(visible);
    importPitchEnvParamButton.setVisible(visible);
    exportPitchEnvParamButton.setVisible(visible);
    importSsgSwEnvParamButton.setVisible(visible);
    exportSsgSwEnvParamButton.setVisible(visible);
    importDetuneParamButton.setVisible(visible);
    exportDetuneParamButton.setVisible(visible);
    targerPadSlider.setVisibleWithLabel(visible);
    uSep003.setVisible(visible);
    importUnisonParamButton.setVisible(visible);
    exportUnisonParamButton.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        auto uSep001Area = rect.removeFromTop(4);
        uSep001.setBounds(uSep001Area);

        layoutMain({ .mainRect = rect, .component = &copyPadParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyPadFromSlider.label, .component = &copyPadFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyPadToSlider.label, .component = &copyPadToSlider });

        auto uSep002Area = rect.removeFromTop(4);
        uSep002.setBounds(uSep002Area);

        layoutMain({ .mainRect = rect, .component = &importLfoParamButton });
        layoutMain({ .mainRect = rect, .component = &exportLfoParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importAmpEnvParamButton });
        layoutMain({ .mainRect = rect, .component = &exportAmpEnvParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importPitchEnvParamButton });
        layoutMain({ .mainRect = rect, .component = &exportPitchEnvParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importSsgSwEnvParamButton });
        layoutMain({ .mainRect = rect, .component = &exportSsgSwEnvParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importDetuneParamButton });
        layoutMain({ .mainRect = rect, .component = &exportDetuneParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .label = &targerPadSlider.label, .component = &targerPadSlider });

        auto uSep003Area = rect.removeFromTop(4);
        uSep003.setBounds(uSep003Area);

        layoutMain({ .mainRect = rect, .component = &importUnisonParamButton });
        layoutMain({ .mainRect = rect, .component = &exportUnisonParamButton });
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

void GuiRhythm::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
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
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiRhythm::copyPadParams(int p, CopyRhythmPad& copyObj) {
    pads[p].copyParams(copyObj);
}

void GuiRhythm::pastePadParams(int p, CopyRhythmPad& copyObj) {
    pads[p].pasteParams(copyObj);
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

void GuiRhythm::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiRhythm::exportUnisonParam() {
    unisonComponent.exportParams();
}
