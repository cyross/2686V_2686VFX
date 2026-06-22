#include "./GuiAdpcm.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Adpcm/ProcessorAdpcmKeys.h"
#include "../../Processor/Adpcm/ProcessorAdpcmValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiAdpcmValues.h"
#include "./GuiAdpcmText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiAdpcmHelpers.h"

// 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM, 13: 4bit ADPCM, 14: 1bit DPCM
static std::vector<SelectItem> qualityItems = {
    {.name = " 1: Raw (32bit)", .value =  1 },
    {.name = " 2: 24-bit PCM",  .value =  2 },
    {.name = " 3: 20-bit PCM",  .value =  3 },
    {.name = " 4: 16-bit PCM",  .value =  4 },
    {.name = " 5: 12-bit PCM",  .value =  5 },
    {.name = " 6: 10-bit PCM",  .value =  6 },
    {.name = " 7: 9-bit PCM",   .value =  7 },
    {.name = " 8: 8-bit PCM",   .value =  8 },
    {.name = " 9: 7-bit PCM",   .value =  9 },
    {.name = "10: 6-bit PCM",   .value = 10 },
    {.name = "11: 5-bit PCM",   .value = 11 },
    {.name = "12: 4-bit PCM",   .value = 12 },
    {.name = "13: 4-bit ADPCM", .value = 13 },
    {.name = "14: 1-bit DPCM",  .value = 14 },
};

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value =  1 },
    {.name = " 2: 55.5kHz",  .value =  2 },
    {.name = " 3: 49.7kHz",  .value =  3 },
    {.name = " 4: 48kHz",    .value =  4 },
    {.name = " 5: 44.1kHz",  .value =  5 },
    {.name = " 6: 33.08kHz", .value =  6 },
    {.name = " 7: 32kHz",    .value =  7 },
    {.name = " 8: 22.05kHz", .value =  8 },
    {.name = " 9: 16kHz",    .value =  9 },
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

void GuiAdpcm::setup()
{
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    const juce::String code = AdpcmPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, AdpcmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(
        juce::Font(juce::FontOptions(18.0f))
    );
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleForm, .invisibleTitle = AdpcmGuiText::Category::invisibleForm, .enableChangeDetailVisible = true });
    qualityCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleQuality, .invisibleTitle = AdpcmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    modeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::mode, .title = AdpcmGuiText::Adpcm::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::rate, .title = AdpcmGuiText::Adpcm::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    // 出力レベル
	levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::level, .title = AdpcmGuiText::Adpcm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::tone, .title = AdpcmGuiText::Adpcm::tone, .isReset = true });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::noise, .title = AdpcmGuiText::Adpcm::noise, .isReset = true });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::noiseFreq, .title = AdpcmGuiText::Adpcm::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::mix , .title = AdpcmGuiText::Adpcm::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Adpcm::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    interpSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::interp, .title = AdpcmGuiText::Adpcm::interp, .items = interpItems, .isReset = true });
    interpSelector.setWantsKeyboardFocus(true);
    interpSelector.setExplicitFocusOrder(++tabOrder);

    optionalCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleOptional, .invisibleTitle = AdpcmGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

    // ループトグルボタン
    loopButton.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::loop, .title = AdpcmGuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::pcmOffset, .title = AdpcmGuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::pcmRatio, .title = AdpcmGuiText::Adpcm::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // パンポット設定
    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visiblePan, .invisibleTitle = AdpcmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + AdpcmPrKey::pan, .title = AdpcmGuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    setupPanBtn(panToLBtn, AdpcmGuiText::Adpcm::Pan::l, tabOrder);
    setupPanBtn(panToCBtn, AdpcmGuiText::Adpcm::Pan::c, tabOrder);
    setupPanBtn(panToRBtn, AdpcmGuiText::Adpcm::Pan::r, tabOrder);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass, AdpcmGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, AdpcmPrKey::ssgSwEnv + AdpcmPrKey::bypass, AdpcmGuiText::SsgSwEnv::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    lfoComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

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
    clearButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            ctx.audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Category::visibleUtil, .invisibleTitle = AdpcmGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    mainGroup.contentCanvas.addAndMakeVisible(uSep001);
    uSep001.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::lfoFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importLfoParamButton.setWantsKeyboardFocus(true);
    importLfoParamButton.setExplicitFocusOrder(++tabOrder);
    importLfoParamButton.onClick = [this] {
        importLfoParam();
        };

    exportLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::lfoFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportLfoParamButton.setWantsKeyboardFocus(true);
    exportLfoParamButton.setExplicitFocusOrder(++tabOrder);
    exportLfoParamButton.onClick = [this] {
        exportLfoParam();
        };

    importAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::ampEnvFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importAmpEnvParamButton.setWantsKeyboardFocus(true);
    importAmpEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importAmpEnvParamButton.onClick = [this] {
        importAmpEnvParam();
        };

    exportAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::ampEnvFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportAmpEnvParamButton.setWantsKeyboardFocus(true);
    exportAmpEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportAmpEnvParamButton.onClick = [this] {
        exportAmpEnvParam();
        };

    importPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::pitchEnvFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importPitchEnvParamButton.setWantsKeyboardFocus(true);
    importPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importPitchEnvParamButton.onClick = [this] {
        importPitchEnvParam();
        };

    exportPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::pitchEnvFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportPitchEnvParamButton.setWantsKeyboardFocus(true);
    exportPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportPitchEnvParamButton.onClick = [this] {
        exportPitchEnvParam();
        };

    importSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::ssgSwEnvFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    importSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importSsgSwEnvParamButton.onClick = [this] {
        importSsgSwEnvParam();
        };

    exportSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::ssgSwEnvFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    exportSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportSsgSwEnvParamButton.onClick = [this] {
        exportSsgSwEnvParam();
        };

    importDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::detuneFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importDetuneParamButton.setWantsKeyboardFocus(true);
    importDetuneParamButton.setExplicitFocusOrder(++tabOrder);
    importDetuneParamButton.onClick = [this] {
        importDetuneParam();
        };

    exportDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::detuneFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportDetuneParamButton.setWantsKeyboardFocus(true);
    exportDetuneParamButton.setExplicitFocusOrder(++tabOrder);
    exportDetuneParamButton.onClick = [this] {
        exportDetuneParam();
        };

    importUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::unisonFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importUnisonParamButton.setWantsKeyboardFocus(true);
    importUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    importUnisonParamButton.onClick = [this] {
        importUnisonParam();
        };

    exportUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::unisonFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportUnisonParamButton.setWantsKeyboardFocus(true);
    exportUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    exportUnisonParamButton.onClick = [this] {
        exportUnisonParam();
        };

    importQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::qualityFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importQualityParamButton.setWantsKeyboardFocus(true);
    importQualityParamButton.setExplicitFocusOrder(++tabOrder);
    importQualityParamButton.onClick = [this] {
        importQualityParam();
        };

    exportQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = AdpcmGuiText::Utility::qualityFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportQualityParamButton.setWantsKeyboardFocus(true);
    exportQualityParamButton.setExplicitFocusOrder(++tabOrder);
    exportQualityParamButton.onClick = [this] {
        exportQualityParam();
        };

    setupGraph();
    updateGraph();
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(AdpcmGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(AdpcmGuiValue::Group::Padding::width, AdpcmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(AdpcmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mmRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mmRect.removeFromTop(AdpcmGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // グラフ用の区画を確保
    layoutGraph(mmRect);
    updateGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutFormCat(mRect);

    layoutOptionalCat(mRect);

    layoutPanCat(mRect);

    fixComponent.layoutComponent(mRect);

    ampEnvComponent.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);
    
    lfoComponent.layoutComponent(mRect);

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

void GuiAdpcm::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
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
    importUnisonParamButton.setVisible(visible);
    exportUnisonParamButton.setVisible(visible);
    importQualityParamButton.setVisible(visible);
    exportQualityParamButton.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        auto uSep001Area = rect.removeFromTop(4);
        uSep001.setBounds(uSep001Area);

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

        layoutMain({ .mainRect = rect, .component = &importUnisonParamButton });
        layoutMain({ .mainRect = rect, .component = &exportUnisonParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importQualityParamButton });
        layoutMain({ .mainRect = rect, .component = &exportQualityParamButton });
    }
}

void GuiAdpcm::layoutFormCat(Rectangle<int>& rect) {
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
        layoutMainPcm({ .rect = rect, .loadPcmBtn = &loadButton, .pcmFileNameLabel = &fileNameLabel, .clearPcmBtn = &clearButton });
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });
    }
}

void GuiAdpcm::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);
    interpSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutMain({ .mainRect = rect, .label = &interpSelector.label, .component = &interpSelector, });
    }
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

    loopButton.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutMain({ .mainRect = rect, .component = &loopButton });
        layoutMain({ .mainRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutMain({ .mainRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
    }
}

void GuiAdpcm::setupGraph()
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

void GuiAdpcm::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiAdpcm::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(AdpcmGuiValue::MainGroup::Graph::height + AdpcmGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(AdpcmGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(AdpcmGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiAdpcm::updateGraph()
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

void GuiAdpcm::setLevel(float level) {
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiAdpcm::importLfoParam() {
    lfoComponent.importParams();
}

void GuiAdpcm::exportLfoParam() {
    lfoComponent.exportParams();
}

void GuiAdpcm::importAmpEnvParam() {
    ampEnvComponent.importParams();
}

void GuiAdpcm::exportAmpEnvParam() {
    ampEnvComponent.exportParams();
}

void GuiAdpcm::importPitchEnvParam() {
    pitchEnvComponent.importParams();
}

void GuiAdpcm::exportPitchEnvParam() {
    pitchEnvComponent.exportParams();
}

void GuiAdpcm::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void GuiAdpcm::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void GuiAdpcm::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void GuiAdpcm::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void GuiAdpcm::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiAdpcm::exportUnisonParam() {
    unisonComponent.exportParams();
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

                modeSelector.setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
                rateSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
                interpSelector.setSelectedItemIndex(lines[2].getIntValue(), juce::sendNotification);
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

                content += juce::String(modeSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(interpSelector.getSelectedItemIndex()) + "\n";

                file.replaceWithText(content);
            }
        });
}
