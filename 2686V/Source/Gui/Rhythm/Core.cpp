#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Rhythm/Keys.h"
#include "../../Processor/Rhythm/Values.h"
#include "../../Core/Const/FileValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiHelpers.h"

static std::vector<SelectItem> qualityItems = {
    {.name = "1: Raw (32bit)", .value = 1 },
    {.name = "2: 24-bit PCM",  .value = 2 },
    {.name = "3: 16-bit PCM",  .value = 3 },
    {.name = "4: 8-bit PCM",   .value = 4 },
    {.name = "5: 5-bit PCM",   .value = 5 },
    {.name = "6: 4-bit PCM",   .value = 6 },
    {.name = "7: 4-bit ADPCM", .value = 7 },
};

static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 22.05kHz", .value = 6 },
    {.name = " 7: 16kHz",    .value = 7 },
    {.name = " 8: 12kHz",    .value = 8 },
    {.name = " 9: 11kHz",    .value = 9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
};

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    juce::Logger::getCurrentLogger()->writeToLog(fileName);
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

    qualityCat.setupHwCategory({ .parent = *this, .title = RhythmGuiText::Category::visibleQuality, .invisibleTitle = RhythmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    modeSelector.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::mode, .title = RhythmGuiText::Rhythm::Pad::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::rate, .title = RhythmGuiText::Rhythm::Pad::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    // 音声ファイルロードボタン
    loadButton.setup({ .parent = *this, .title = RhythmGuiText::File::load, .isReset = false });
    loadButton.addListener(&ctx.editor);
    loadButton.setWantsKeyboardFocus(true);
    loadButton.setExplicitFocusOrder(++tabOrder);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centred);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // パッド音声アンロード
    clearButton.setup({ .parent = *this, .title = RhythmGuiText::File::clear, .isReset = false });
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

    optionalCat.setupSwCategory({ .parent = *this, .title = RhythmGuiText::Category::visibleOptional, .invisibleTitle = RhythmGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pcmOffset, .title = RhythmGuiText::Rhythm::Pad::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pcmRatio, .title = RhythmGuiText::Rhythm::Pad::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // Vol
    volSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::volume, .title = RhythmGuiText::Rhythm::Pad::vol, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::oneShot, .title = RhythmGuiText::Rhythm::Pad::oneShot, .isReset = true });
    oneShotButton.setWantsKeyboardFocus(true);
    oneShotButton.setExplicitFocusOrder(++tabOrder);

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::note, .title = RhythmGuiText::Rhythm::Pad::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.setWantsKeyboardFocus(true);
    noteSlider.setExplicitFocusOrder(++tabOrder);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    panCat.setupHwCategory({ .parent = *this, .title = RhythmGuiText::Category::visiblePan, .invisibleTitle = RhythmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    // パンポット
    panSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pan, .title = RhythmGuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);
    panSlider.setRange(0.0f, 1.0f);

    setupPanBtn(panToLBtn, RhythmGuiText::Rhythm::Pad::Pan::l, tabOrder);
    setupPanBtn(panToCBtn, RhythmGuiText::Rhythm::Pad::Pan::c, tabOrder);
    setupPanBtn(panToRBtn, RhythmGuiText::Rhythm::Pad::Pan::r, tabOrder);

    adsrCat.setupSwCategory({ .parent = *this, .title = RhythmGuiText::Category::visibleAdsr, .invisibleTitle = RhythmGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    adsrBypassButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::adsr + RhythmPrKey::bypass, .title = RhythmGuiText::Rhythm::Pad::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::stl, .title = RhythmGuiText::Rhythm::Pad::Adsr::stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::ar, .title = RhythmGuiText::Rhythm::Pad::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::dr , .title = RhythmGuiText::Rhythm::Pad::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::sl, .title = RhythmGuiText::Rhythm::Pad::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::rr, .title = RhythmGuiText::Rhythm::Pad::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setupSwCategory({ .parent = *this, .title = RhythmGuiText::Category::visiblePitchAdsr, .invisibleTitle = RhythmGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::pitchAdsr + RhythmPrKey::bypass, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::ar, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::dr , .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::rr, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::stl, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::atl, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::ssl, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::rll, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    setupGraph();
    updateGraph();
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto padRect = content.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);

    padRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    // グラフ用の区画を確保
    layoutGraph(padRect);
    updateGraph();

    layoutRowRhythmPadPcmFile({ .rect = padRect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton });

    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });

    layoutOptionalCat(padRect);

    layoutPanCat(padRect);

    layoutAdsrCat(padRect);

	layoutPitchEnvCat(padRect);

    layoutQualityCat(padRect);
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

    if (visibleQuality)
    {
        layoutRow({ .rowRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
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

void RhythmPadGui::layoutAdsrCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &adsrCat });

    bool visible = adsrCat.isDetailVisible();

    adsrBypassButton.setVisible(visible);
    startLevelSlider.setVisibleWithLabel(visible);
    attackSlider.setVisibleWithLabel(visible);
    decaySlider.setVisibleWithLabel(visible);
    sustainSlider.setVisibleWithLabel(visible);
    releaseSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &adsrBypassButton });
        layoutMain({ .mainRect = rect, .label = &startLevelSlider.label, .component = &startLevelSlider });
        layoutMain({ .mainRect = rect, .label = &attackSlider.label, .component = &attackSlider });
        layoutMain({ .mainRect = rect, .label = &decaySlider.label, .component = &decaySlider });
        layoutMain({ .mainRect = rect, .label = &sustainSlider.label, .component = &sustainSlider });
        layoutMain({ .mainRect = rect, .label = &releaseSlider.label, .component = &releaseSlider });
    }
}

void RhythmPadGui::layoutPitchEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &pitchAdsrCat });

    bool visible = pitchAdsrCat.isDetailVisible();

    pitchAdsrBypassButton.setVisible(visible);
    pitchAttackSlider.setVisibleWithLabel(visible);
    pitchDecaySlider.setVisibleWithLabel(visible);
    pitchReleaseSlider.setVisibleWithLabel(visible);
    pitchStartLevelSlider.setVisibleWithLabel(visible);
    pitchAttackLevelSlider.setVisibleWithLabel(visible);
    pitchSustainLevelSlider.setVisibleWithLabel(visible);
    pitchReleaseLevelSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &pitchAdsrBypassButton });
        layoutMain({ .mainRect = rect, .label = &pitchAttackSlider.label, .component = &pitchAttackSlider });
        layoutMain({ .mainRect = rect, .label = &pitchDecaySlider.label, .component = &pitchDecaySlider });
        layoutMain({ .mainRect = rect, .label = &pitchReleaseSlider.label, .component = &pitchReleaseSlider });
        layoutMain({ .mainRect = rect, .label = &pitchStartLevelSlider.label, .component = &pitchStartLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchAttackLevelSlider.label, .component = &pitchAttackLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchSustainLevelSlider.label, .component = &pitchSustainLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchReleaseLevelSlider.label, .component = &pitchReleaseLevelSlider });
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

    adsrBypassButton.onStateChange = repaintGraph;
    pitchAdsrBypassButton.onStateChange = repaintGraph;

    startLevelSlider.onValueChange = repaintGraph;
    attackSlider.onValueChange = repaintGraph;
    decaySlider.onValueChange = repaintGraph;
    sustainSlider.onValueChange = repaintGraph;
    releaseSlider.onValueChange = repaintGraph;

    pitchAttackSlider.onValueChange = repaintGraph;
    pitchDecaySlider.onValueChange = repaintGraph;
    pitchReleaseSlider.onValueChange = repaintGraph;
    pitchStartLevelSlider.onValueChange = repaintGraph;
    pitchAttackLevelSlider.onValueChange = repaintGraph;
    pitchSustainLevelSlider.onValueChange = repaintGraph;
    pitchReleaseLevelSlider.onValueChange = repaintGraph;

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
        graph.updateBypass(pitchAdsrBypassButton.getToggleState());

        graph.updatePitchEnv(
            pitchAttackSlider,
            pitchDecaySlider,
            pitchReleaseSlider,
            pitchStartLevelSlider,
            pitchAttackLevelSlider,
            pitchSustainLevelSlider,
            pitchReleaseLevelSlider,
            p_curveCore,
            isCurveMode,
            0
        );
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        graph.updateBypass(adsrBypassButton.getToggleState());

        graph.updateAmpEnv(
            startLevelSlider,
            attackSlider,
            decaySlider,
            sustainSlider,
            releaseSlider,
            p_curveCore,
            isCurveMode,
            0
        );
    }
}

void GuiRhythm::setup()
{
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;

    // パッド名定義
    const std::array<juce::String, 8> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    levelSlider.setup({ .parent = *this, .id = code + RhythmPrKey::level, .title = RhythmGuiText::Rhythm::vol, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    unisonComponent.setupComponent(*this, code, tabOrder);

    // Setup 8 Pads
    for (int i = 0; i < 8; ++i)
    {
        pads[i].setup(*this, i, padNames[i], tabOrder);
    }
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
    auto mRect = mainArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);
    mRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(RhythmGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    unisonComponent.layoutComponent(mRect);

    auto topPadsArea = pageArea.removeFromTop(RhythmGuiValue::Pad::height);
    auto bottomPadsArea = pageArea.removeFromTop(RhythmGuiValue::Pad::height);

    // Remaining area for 8 pads
    applyPads(topPadsArea, topPadsArea.getWidth() / 4, 0, 4);
    applyPads(bottomPadsArea, bottomPadsArea.getWidth() / 4, 4, 4);
}

void GuiRhythm::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    for (int i = 0; i < 8; ++i)
    {
        pads[i].removeLoadButtonListener(editor);
    }
}

void GuiRhythm::buttonClicked(juce::Button* button, juce::AudioFormatManager& formatManager, std::unique_ptr<juce::FileChooser>& fileChooser)
{
    for (int i = 0; i < 8; ++i)
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
                        // Load to specific pad index
                        ctx.audioProcessor.loadRhythmFile(file, i);

                        // Update label
                        pads[i].updatePadFileName(file.getFileName());

                        ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
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
    for (int i = 0; i < 8; i++)
    {
        this->ctx.audioProcessor.unloadRhythmFile(i);
        updatePadFileName(i, Io::empty);
    }
}
