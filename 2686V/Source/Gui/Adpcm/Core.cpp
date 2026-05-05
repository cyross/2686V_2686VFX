#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

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
    {.name = "1: 96kHz",    .value = 1 },
    {.name = "2: 55.5kHz",  .value = 2 },
    {.name = "3: 48kHz",    .value = 3 },
    {.name = "4: 44.1kHz",  .value = 4 },
    {.name = "5: 22.05kHz", .value = 5 },
    {.name = "6: 16kHz",    .value = 6 },
    {.name = "7: 8kHz",     .value = 7 },
};

void GuiAdpcm::setup()
{
    const juce::String code = PrKey::Prefix::adpcm;
    int tabOrder = 1;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });

    modeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::mode, .title = GuiText::Adpcm::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::rate, .title = GuiText::Adpcm::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    // 出力レベル
	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::level, .title = GuiText::Adpcm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    // ループトグルボタン
    loopButton.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::loop, .title = GuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + PrKey::Post::Adpcm::pcmOffset, .title = GuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + PrKey::Post::Adpcm::pcmRatio, .title = GuiText::Adpcm::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // パンポット設定
    panCat.setup({ .parent = *this, .title = GuiText::Category::pan });

    panSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::pan, .title = GuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    addAndMakeVisible(btnPanL); btnPanL.setButtonText(GuiText::Adpcm::Pan::l); btnPanL.addListener(&ctx.editor);
    btnPanL.setWantsKeyboardFocus(true);
    btnPanL.setExplicitFocusOrder(++tabOrder);

    addAndMakeVisible(btnPanC); btnPanC.setButtonText(GuiText::Adpcm::Pan::c); btnPanC.addListener(&ctx.editor);
    btnPanC.setWantsKeyboardFocus(true);
    btnPanC.setExplicitFocusOrder(++tabOrder);

    addAndMakeVisible(btnPanR); btnPanR.setButtonText(GuiText::Adpcm::Pan::r); btnPanR.addListener(&ctx.editor);
    btnPanR.setWantsKeyboardFocus(true);
    btnPanR.setExplicitFocusOrder(++tabOrder);

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    adsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::ar, .title = GuiText::Adpcm::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::dr, .title = GuiText::Adpcm::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::sl, .title = GuiText::Adpcm::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::rr, .title = GuiText::Adpcm::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    paramGroup.setup(*this, GuiText::Group::adpcmParams);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = *this, .title = GuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    clearButton.setup({ .parent = *this, .title = GuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
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

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMainCategory({ .mainRect = mRect, .label = &qualityCat });
    layoutMain({ .mainRect = mRect, .label = &modeSelector.label, .component = &modeSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, });
    layoutMainCategory({ .mainRect = mRect, .label = &mainCat });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });
    layoutMain({ .mainRect = mRect, .component = &loopButton });
    layoutMain({ .mainRect = mRect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
    layoutMain({ .mainRect = mRect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
    layoutMainCategory({ .mainRect = mRect, .label = &panCat });
    layoutMain({ .mainRect = mRect, .label = &panSlider.label, .component = &panSlider });
    layoutMainThreeComps({ .rect = mRect, .comp1 = &btnPanL, .comp2 = &btnPanC, .comp3 = &btnPanR, });
    layoutMainCategory({ .mainRect = mRect, .label = &adsrCat });
    layoutMain({ .mainRect = mRect, .component = &adsrBypassButton });
    layoutMain({ .mainRect = mRect, .label = &attackSlider.label, .component = &attackSlider });
    layoutMain({ .mainRect = mRect, .label = &decaySlider.label, .component = &decaySlider });
    layoutMain({ .mainRect = mRect, .label = &sustainSlider.label, .component = &sustainSlider });
    layoutMain({ .mainRect = mRect, .label = &releaseSlider.label, .component = &releaseSlider });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    auto headerArea = pageArea.removeFromTop(GuiValue::Adpcm::File::height);

    paramGroup.setBounds(headerArea);

    auto inner = headerArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    inner.removeFromTop(GuiValue::Group::TitlePaddingTop);

    auto fileRow = inner.removeFromTop(30);
    // 左にロードボタン
    loadButton.setBounds(fileRow.removeFromLeft(100));
    fileRow.removeFromLeft(10);
    // 右にクリアボタン
    clearButton.setBounds(fileRow.removeFromRight(60));
    fileRow.removeFromRight(10);
    // 中間にファイル名ラベル
    fileNameLabel.setBounds(fileRow);
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
    return button == &btnPanL;
}

bool GuiAdpcm::isBtnPanC(juce::Button* button)
{
    return button == &btnPanC;
}

bool GuiAdpcm::isBtnPanR(juce::Button* button)
{
    return button == &btnPanR;
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
