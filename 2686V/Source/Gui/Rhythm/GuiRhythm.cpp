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
    {.name = juce::String("") + "3: ガウス補完 (Gaussian)", .value = 3 },
    {.name = juce::String("") + "4: ZOH (Zero-Order Hold)", .value = 4 },
    {.name = juce::String("") + "5: コサイン補間 (Cosine)", .value = 5 },
    {.name = juce::String("") + "6: B-スプライン補間 (B-Spline)", .value = 6 },
    {.name = juce::String("") + "7: ラグランジュ補間 (Lagrange)", .value = 7 }
};

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

void RhythmPadGui::setup(juce::Component &parent, int index, juce::String padName, int& tabOrder)
{
    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

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

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Category::visibleForm, .invisibleTitle = RhythmGuiText::Category::invisibleForm, .enableChangeDetailVisible = true });
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

    loopPointEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::loopPointEnable, .title = RhythmGuiText::Rhythm::Pad::loopPointEnable, .isReset = true });
    loopPointEnableButton.setWantsKeyboardFocus(true);
    loopPointEnableButton.setExplicitFocusOrder(++tabOrder);

    loopPointStartSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::loopPointStart, .title = RhythmGuiText::Rhythm::Pad::loopPointStart, .isReset = true });
    loopPointStartSlider.setWantsKeyboardFocus(true);
    loopPointStartSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEndSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = padPrefix + RhythmPrKey::Pad::loopPointEnd, .title = RhythmGuiText::Rhythm::Pad::loopPointEnd, .isReset = true });
    loopPointEndSlider.setWantsKeyboardFocus(true);
    loopPointEndSlider.setExplicitFocusOrder(++tabOrder);

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

    setupPanBtn(mainGroup.contentCanvas, panToLBtn, RhythmGuiText::Rhythm::Pad::Pan::l, tabOrder);
    setupPanBtn(mainGroup.contentCanvas, panToCBtn, RhythmGuiText::Rhythm::Pad::Pan::c, tabOrder);
    setupPanBtn(mainGroup.contentCanvas, panToRBtn, RhythmGuiText::Rhythm::Pad::Pan::r, tabOrder);

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

    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });

    layoutFormCat(padRect);

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
    qualityCat.setVisible(visible);
    interpSelector.setVisibleWithLabel(visible);
    panCat.setVisible(visible);
    noteSlider.setVisibleWithLabel(visible);
    modeSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
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
    graphSeparator.setVisible(visible);
}

void RhythmPadGui::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    loadButton.setVisible(visible);
    fileNameLabel.setVisible(visible);
    clearButton.setVisible(visible);
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
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });
    }
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

    graphBtnSsg.setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    auto repaintGraph = [this]() { updateGraph(); };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

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
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

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

void RhythmPadGui::copyParams(CopyRhythmPad& copyObj) {
    copyObj.base.level = volSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();
    copyObj.isOneShot = oneShotButton.getToggleState();
    copyObj.noteNumber = noteSlider.getValue();
    copyObj.pcm.pcmOffset = pcmOffsetSlider.getValue();
    copyObj.pcm.pcmRatio = pcmRatioSlider.getValue();
    copyObj.quality.mode = modeSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();
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
    modeSelector.setSelectedId(copyObj.quality.mode);
    rateSelector.setSelectedId(copyObj.quality.rate);
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

                modeSelector.setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
                rateSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
                interpSelector.setSelectedItemIndex(lines[2].getIntValue(), juce::sendNotification);
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

                content += juce::String(modeSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(interpSelector.getSelectedItemIndex()) + "\n";

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

                modeSelector.setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
                rateSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
                interpSelector.setSelectedItemIndex(lines[2].getIntValue(), juce::sendNotification);
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

                content += juce::String(modeSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(interpSelector.getSelectedItemIndex()) + "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiRhythm::setup()
{
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;

    // パッド名定義
    const std::array<juce::String, RhythmPrValue::pads> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

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

    importToneNoiseParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::toneNoiseFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
    importToneNoiseParamButton.setWantsKeyboardFocus(true);
    importToneNoiseParamButton.setExplicitFocusOrder(++tabOrder);
    importToneNoiseParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importToneNoiseParam(padIndex);
        };

    exportToneNoiseParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::toneNoiseFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportToneNoiseParamButton.setWantsKeyboardFocus(true);
    exportToneNoiseParamButton.setExplicitFocusOrder(++tabOrder);
    exportToneNoiseParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportToneNoiseParam(padIndex);
        };

    importLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::lfoFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
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

    importAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::ampEnvFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
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

    importPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::pitchEnvFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
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

    importSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::ssgSwEnvFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
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

    importDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::detuneFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
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

    importQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::qualityFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
    importQualityParamButton.setWantsKeyboardFocus(true);
    importQualityParamButton.setExplicitFocusOrder(++tabOrder);
    importQualityParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importQualityParam(padIndex);
        };

    exportQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::qualityFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportQualityParamButton.setWantsKeyboardFocus(true);
    exportQualityParamButton.setExplicitFocusOrder(++tabOrder);
    exportQualityParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportQualityParam(padIndex);
        };

    importPcmPlayParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::pcmPlayFileImport, .bgColor = juce::Colours::hotpink.darker(0.5f), .isReset = false, .isResized = false });
    importPcmPlayParamButton.setWantsKeyboardFocus(true);
    importPcmPlayParamButton.setExplicitFocusOrder(++tabOrder);
    importPcmPlayParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        importPcmPlayParam(padIndex);
        };

    exportPcmPlayParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::pcmPlayFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportPcmPlayParamButton.setWantsKeyboardFocus(true);
    exportPcmPlayParamButton.setExplicitFocusOrder(++tabOrder);
    exportPcmPlayParamButton.onClick = [this] {
        int padIndex = (int)targerPadSlider.getValue() - 1;

        exportPcmPlayParam(padIndex);
        };

    targerPadSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Pad", .isReset = false });
    targerPadSlider.setRange(1.0, 8.0, 1.0);
    targerPadSlider.setNumDecimalPlacesToDisplay(0);
    targerPadSlider.setValue(1, juce::sendNotification);
    targerPadSlider.setWantsKeyboardFocus(true);
    targerPadSlider.setExplicitFocusOrder(++tabOrder);

    mainGroup.contentCanvas.addAndMakeVisible(uSep003);
    uSep003.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = RhythmGuiText::Utility::unisonFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
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

    viewModeComp.setupComponent(*this, tabOrder);
    viewModeComp.onChangeViewMode = [this] (GuiComponentViewModes mode) {
        viewMode = mode;

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

    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    viewModeComp.layoutComponent(mmRect);

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
    importToneNoiseParamButton.setVisible(visible);
    exportToneNoiseParamButton.setVisible(visible);
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
    importQualityParamButton.setVisible(visible);
    exportQualityParamButton.setVisible(visible);
    importPcmPlayParamButton.setVisible(visible);
    exportPcmPlayParamButton.setVisible(visible);
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

        layoutMainTwoComps({ .rect = rect, .comp1 = &importToneNoiseParamButton, .comp2 = &exportToneNoiseParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importLfoParamButton, .comp2 = &exportLfoParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importAmpEnvParamButton, .comp2 = &exportAmpEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importPitchEnvParamButton, .comp2 = &exportPitchEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importSsgSwEnvParamButton, .comp2 = &exportSsgSwEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importDetuneParamButton, .comp2 = &exportDetuneParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importQualityParamButton, .comp2 = &exportQualityParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importPcmPlayParamButton, .comp2 = &exportPcmPlayParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .label = &targerPadSlider.label, .component = &targerPadSlider });

        auto uSep003Area = rect.removeFromTop(4);
        uSep003.setBounds(uSep003Area);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importUnisonParamButton, .comp2 = &exportUnisonParamButton });
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
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
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

void GuiRhythm::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiRhythm::exportUnisonParam() {
    unisonComponent.exportParams();
}
