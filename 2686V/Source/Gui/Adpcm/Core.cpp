#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Adpcm/Keys.h"
#include "../../Processor/Adpcm/Values.h"

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
    {.name = " 1: 96kHz",    .value =  1 },
    {.name = " 2: 55.5kHz",  .value =  2 },
    {.name = " 3: 49.7kHz",  .value =  3 },
    {.name = " 4: 48kHz",    .value =  4 },
    {.name = " 5: 44.1kHz",  .value =  5 },
    {.name = " 6: 22.05kHz", .value =  6 },
    {.name = " 7: 16kHz",    .value =  7 },
    {.name = " 8: 12kHz",    .value =  8 },
    {.name = " 9: 11kHz",    .value =  9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
};

// DT (デチューン1) 用のコンボボックスアイテム
// レジスタ仕様: 0=0, 1=+1, 2=+2, 3=+3, 4=0, 5=-1, 6=-2, 7=-3
static std::vector<SelectItem> dtItems = {
    {.name = " 0", .value = 1 },
    {.name = "-3", .value = 2 },
    {.name = "-2", .value = 3 },
    {.name = "-1", .value = 4 },
    {.name = " 0", .value = 5 }, // 実質0ですが、レジスタ4として一応用意
    {.name = "+1", .value = 6 },
    {.name = "+2", .value = 7 },
    {.name = "+3", .value = 8 }
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

    mainGroup.setup(*this, AdpcmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    qualityCat.setup({ .parent = *this, .title = AdpcmGuiText::Category::visibleQuality, .invisibleTitle = AdpcmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    modeSelector.setup({ .parent = *this, .id = code + AdpcmPrKey::mode, .title = AdpcmGuiText::Adpcm::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + AdpcmPrKey::rate, .title = AdpcmGuiText::Adpcm::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    // 出力レベル
	levelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::level, .title = AdpcmGuiText::Adpcm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    // ループトグルボタン
    loopButton.setup({ .parent = *this, .id = code + AdpcmPrKey::loop, .title = AdpcmGuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + AdpcmPrKey::pcmOffset, .title = AdpcmGuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + AdpcmPrKey::pcmRatio, .title = AdpcmGuiText::Adpcm::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // パンポット設定
    panCat.setup({ .parent = *this, .title = AdpcmGuiText::Category::visiblePan, .invisibleTitle = AdpcmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::pan, .title = AdpcmGuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    setupPanBtn(panToLBtn, AdpcmGuiText::Adpcm::Pan::l, tabOrder);
    setupPanBtn(panToCBtn, AdpcmGuiText::Adpcm::Pan::c, tabOrder);
    setupPanBtn(panToRBtn, AdpcmGuiText::Adpcm::Pan::r, tabOrder);

    adsrCat.setup({ .parent = *this, .title = AdpcmGuiText::Category::visibleAdsr, .invisibleTitle = AdpcmGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    adsrBypassButton.setup({ .parent = *this, .id = code + AdpcmPrKey::adsr + AdpcmPrKey::bypass, .title = AdpcmGuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::stl, .title = AdpcmGuiText::Adpcm::Adsr::stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::ar, .title = AdpcmGuiText::Adpcm::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::dr, .title = AdpcmGuiText::Adpcm::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::sl, .title = AdpcmGuiText::Adpcm::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::rr, .title = AdpcmGuiText::Adpcm::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setup({ .parent = *this, .title = AdpcmGuiText::Category::visiblePitchAdsr, .invisibleTitle = AdpcmGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass, .title = AdpcmGuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::ar, .title = AdpcmGuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::dr , .title = AdpcmGuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::rr, .title = AdpcmGuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::stl, .title = AdpcmGuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::atl, .title = AdpcmGuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::ssl, .title = AdpcmGuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::rll, .title = AdpcmGuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setup({ .parent = *this, .title = AdpcmGuiText::Category::visibleDetune, .invisibleTitle = AdpcmGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });

    dt1.setup({ .parent = *this, .id = code + AdpcmPrKey::dt, .title = AdpcmGuiText::Detune::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + AdpcmPrKey::dt2, .title = AdpcmGuiText::Detune::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = AdpcmGuiText::Category::visibleMvol, .invisibleTitle = AdpcmGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = AdpcmPrKey::masterVol, .title = AdpcmGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = *this, .title = AdpcmGuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    clearButton.setup({ .parent = *this, .title = AdpcmGuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            ctx.audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(AdpcmGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(AdpcmGuiValue::Group::Padding::width, AdpcmGuiValue::Group::Padding::height);
    mRect.removeFromTop(AdpcmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutMainPcm({ .rect = mRect, .loadPcmBtn = &loadButton, .pcmFileNameLabel = &fileNameLabel, .clearPcmBtn = &clearButton });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });
    layoutMain({ .mainRect = mRect, .component = &loopButton });
    layoutMain({ .mainRect = mRect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
    layoutMain({ .mainRect = mRect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });

    layoutPanCat(mRect);

    layoutAdsrCat(mRect);

    layoutPitchEnvCat(mRect);

    layoutDetuneCat(mRect);

    layoutQualityCat(mRect);

    layoutMvolCat(mRect);
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

void GuiAdpcm::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void GuiAdpcm::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
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

void GuiAdpcm::layoutAdsrCat(juce::Rectangle<int>& rect)
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

void GuiAdpcm::layoutPitchEnvCat(juce::Rectangle<int>& rect)
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

void GuiAdpcm::layoutDetuneCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &detuneCat });

    bool visible = detuneCat.isDetailVisible();

    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
    }
}
