#include "./GuiAdpcm.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Adpcm/ProcessorAdpcmKeys.h"
#include "../../Processor/Adpcm/ProcessorAdpcmValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiAdpcmValues.h"
#include "./GuiAdpcmText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiAdpcmHelpers.h"

void GuiAdpcm::setup()
{
    auto setupPanBtn = [this](juce::Component& parent, GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            parent.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    const juce::String code = AdpcmPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, AdpcmGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleForm, .invisibleTitle = AdpcmGuiText::Category::invisibleForm, .detailVisible = true, .enableChangeDetailVisible = true });

	qualityPcmComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    // 出力レベル
    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::tone, .title = AdpcmGuiText::Adpcm::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::noise, .title = AdpcmGuiText::Adpcm::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::freq, .title = AdpcmGuiText::Adpcm::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Tn::mix , .title = AdpcmGuiText::Adpcm::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::toTone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::toNoise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    optionalCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleOptional, .invisibleTitle = AdpcmGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

    // ループトグルボタン
    loopButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::loop, .title = AdpcmGuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    loopPointEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::lpEnable, .title = AdpcmGuiText::Adpcm::loopPointEnable, .isReset = true });
    loopPointEnableButton.setWantsKeyboardFocus(true);
    loopPointEnableButton.setExplicitFocusOrder(++tabOrder);

    loopPointStartSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::lpStart, .title = AdpcmGuiText::Adpcm::loopPointStart, .isReset = true });
    loopPointStartSlider.setWantsKeyboardFocus(true);
    loopPointStartSlider.setExplicitFocusOrder(++tabOrder);

    loopPointEndSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::lpEnd, .title = AdpcmGuiText::Adpcm::loopPointEnd, .isReset = true });
    loopPointEndSlider.setWantsKeyboardFocus(true);
    loopPointEndSlider.setExplicitFocusOrder(++tabOrder);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::pcmOffset, .title = AdpcmGuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + CPK::pcmRatio, .title = AdpcmGuiText::Adpcm::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // パンポット設定
    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visiblePan, .invisibleTitle = AdpcmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::pan, .title = AdpcmGuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = AdpcmGuiText::Adpcm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(0.0f, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = AdpcmGuiText::Adpcm::Pan::c, .isReset = false});
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0.5f, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = AdpcmGuiText::Adpcm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1.0f, juce::sendNotification);
        };

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::pitchAdsr + CPK::bypass, AdpcmGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv + CPK::bypass, AdpcmGuiText::SsgSwEnv::bypass);

	ssgSwEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, AdpcmGuiText::SsgSwEnv11::bypass);

    ssgSwPEnv11Component.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, AdpcmGuiText::SsgSwPEnv11::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, ctx.audioProcessor.modWavePaths[code]);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = mainGroup.contentCanvas, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    clearButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            ctx.audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    formSeparator.setupComponent(mainGroup.contentCanvas);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleUtil, .invisibleTitle = AdpcmGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    ieToneNoise.setupComponent(mainGroup.contentCanvas, tabOrder, "Tone/Noise");
    ieToneNoise.onClickImport = [this] { importToneNoiseParam(); };
    ieToneNoise.onClickExport = [this] { exportToneNoiseParam(); };

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", lfoComponent);

    ieDetune.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Detune", mulDetuneComponent);

    ieAmpEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);

    iePitchEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Pitch Env", pitchEnvComponent);

    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);

    ieSsgSwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW Env", ssgSwEnvComponent);

    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11Component);

    ieSsgSwPEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11Component);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    iePcmPlay.setupComponent(mainGroup.contentCanvas, tabOrder, "PCM Play");
    iePcmPlay.onClickImport = [this] { importPcmPlayParam(); };
    iePcmPlay.onClickExport = [this] { exportPcmPlayParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    setupGraph();
    updateGraph();
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(AdpcmGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(AdpcmGuiValue::Group::Padding::width, AdpcmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(AdpcmGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフ用の区画を確保
    layoutGraph(mmRect);
    updateGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

	levelComponent.layoutComponent(mRect);

    layoutFormCat(mRect);

    layoutOptionalCat(mRect);

    layoutPanCat(mRect);

    ampEnvComponent.layoutComponent(mRect);

    ssgHwEnv.layoutComponent(mRect);
    modComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    ssgSwEnv11Component.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwPEnv11Component.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);
    
    lfoComponent.layoutComponent(mRect);

    fixComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);
}

void GuiAdpcm::updateFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

bool GuiAdpcm::isThis(juce::Button* button)
{
    return button == &loadButton;
}

bool GuiAdpcm::isBtnPanL(juce::Button* button)
{
    return button == &panToLBtn;
}

bool GuiAdpcm::isBtnPanC(juce::Button* button)
{
    return button == &panToCBtn;
}

bool GuiAdpcm::isBtnPanR(juce::Button* button)
{
    return button == &panToRBtn;
}

void GuiAdpcm::setPan(float pan)
{
    panSlider.setValue(pan);
}

void GuiAdpcm::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

void GuiAdpcm::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiAdpcm::initParams()
{
    this->ctx.audioProcessor.initParams("ADPCM_");
    this->ctx.audioProcessor.unloadAdpcmFile();
    updateFileName(Io::empty);
}

void GuiAdpcm::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    ieToneNoise.setVisible(visible);
    ieLfo.setVisible(visible);
    ieDetune.setVisible(visible);
    ieAmpEnv.setVisible(visible);
    iePitchEnv.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgSwEnv.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    iePcmPlay.setVisible(visible);
    ieChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

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
        iePcmPlay.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);
    }
}

void GuiAdpcm::layoutFormCat(Rectangle<int>& rect) {
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
        layoutMainPcm({ .rect = rect, .loadPcmBtn = &loadButton, .pcmFileNameLabel = &fileNameLabel, .clearPcmBtn = &clearButton });
        formSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });
    }
}

void GuiAdpcm::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityPcmComponent.layoutComponent(rect);
}

void GuiAdpcm::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn, });
    }
}

void GuiAdpcm::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);
    loopButton.setVisible(visible);
    loopPointEnableButton.setVisible(visible);
    loopPointStartSlider.setVisibleWithLabel(visible);
    loopPointEndSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutMain({ .mainRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutMain({ .mainRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
        layoutMain({ .mainRect = rect, .component = &loopButton });
        layoutMain({ .mainRect = rect, .component = &loopPointEnableButton });
        layoutMain({ .mainRect = rect, .label = &loopPointStartSlider.label, .component = &loopPointStartSlider, });
        layoutMain({ .mainRect = rect, .label = &loopPointEndSlider.label, .component = &loopPointEndSlider, });
    }
}

void GuiAdpcm::setupGraph()
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

void GuiAdpcm::setGraphMode(GraphMode mode)
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

void GuiAdpcm::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(AdpcmGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator.layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(AdpcmGuiValue::MainGroup::Graph::ButtonHeight);
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
void GuiAdpcm::updateGraph()
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

void GuiAdpcm::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiAdpcm::importToneNoiseParam() {
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

void GuiAdpcm::exportToneNoiseParam() {
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

void GuiAdpcm::importQualityParam() {
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

void GuiAdpcm::exportQualityParam() {
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

void GuiAdpcm::importPcmPlayParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultPcmPlayParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importPcmPlayParamFile, defaultDir, Io::ExtensionGlob::PcmPlayParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 5) return;

                pcmOffsetSlider.setValue(lines[0].getFloatValue(), juce::sendNotification);
                pcmRatioSlider.setValue(lines[1].getFloatValue(), juce::sendNotification);
                loopPointEnableButton.setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
                loopPointStartSlider.setValue(lines[3].getFloatValue(), juce::sendNotification);
                loopPointEndSlider.setValue(lines[4].getFloatValue(), juce::sendNotification);
            }
        });
}

void GuiAdpcm::exportPcmPlayParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultPcmPlayParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportPcmPlayParamFile, defaultDir.getChildFile("default.pcmPlay"), Io::ExtensionGlob::PcmPlayParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(pcmOffsetSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(pcmRatioSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(loopPointEnableButton.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(loopPointStartSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(loopPointEndSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiAdpcm::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::adpcmParam);
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

                // Form
                if (fileNameLabel.getText() != lines[index]) {
                    ctx.audioProcessor.unloadAdpcmFile();
                }

                fileNameLabel.setText(lines[index++], juce::dontSendNotification);

                if (fileNameLabel.getText().isNotEmpty()) {
                    ctx.audioProcessor.loadAdpcmFile(fileNameLabel.getText());
                }

                // Optional
                loopButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

                // Tone/Noise
                toneSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
                noiseSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
                noiseFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
                mixSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

                // Pan
                panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

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
                unisonComponent.setImportingParams(lines, index);

                // MODULATION は後から足したので、旧フォーマットとの互換のため
                // 行が無ければ既定のままにする。
                if (index < lines.size()) {
                    modComponent.setImportingBaseParams(lines, index);
                    modComponent.setImportingShapeParam(lines, index);
                }
            }
        });
}

void GuiAdpcm::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::adpcmParam), Io::ExtensionGlob::adpcmParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                // Level
                content += levelComponent.getExportedParams();

                // Form
                content += fileNameLabel.getText() + "\n";

                // Optional
                content += juce::String(loopButton.getToggleState() ? 1 : 0) + "\n";

                // Tone/Noise
                content += juce::String(toneSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(noiseSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(noiseFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(mixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Pan
                content += juce::String(panSlider.getValue(), Global::floatDecimalPlaces) + "\n";

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
                content += unisonComponent.getExportedParams();

                // MODULATION (旧フォーマットと互換を保つため末尾に置く)
                content += modComponent.getExportedBaseParams();
                content += modComponent.getExportedShapeParam();

                file.replaceWithText(content);
            }
        });

}
