#include "GuiAdpcm.h"
#include "../processor/PluginProcessor.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"
#include "../gui/GuiStructs.h"

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

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    modeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::mode, .title = GuiText::Adpcm::quality, .items = qualityItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::rate, .title = GuiText::Adpcm::rate, .items = rateItems, .isReset = true });

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    // 出力レベル
	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::level, .title = GuiText::Adpcm::level, .isReset = true });

    // パンポット設定
    panCat.setup({ .parent = *this, .title = GuiText::Category::pan });
    panSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::pan, .title = GuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    addAndMakeVisible(btnPanL); btnPanL.setButtonText(GuiText::Adpcm::Pan::l); btnPanL.addListener(&ctx.editor);
    addAndMakeVisible(btnPanC); btnPanC.setButtonText(GuiText::Adpcm::Pan::c); btnPanC.addListener(&ctx.editor);
    addAndMakeVisible(btnPanR); btnPanR.setButtonText(GuiText::Adpcm::Pan::r); btnPanR.addListener(&ctx.editor);

    // ループトグルボタン
	loopButton.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::loop, .title = GuiText::Adpcm::loop, .isReset = true });

	paramGroup.setup(*this, GuiText::Group::adpcmParams);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = *this, .title = GuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
	clearButton.setup({ .parent = *this, .title = GuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.onClick = [this]
    {
        // 1. プロセッサーのアンロード関数を呼ぶ
        ctx.audioProcessor.unloadAdpcmFile();

        // 2. ラベル表示をクリア
        fileNameLabel.setText(Io::empty, juce::dontSendNotification);
    };

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + PrKey::Post::Adpcm::pcmOffset, .title = GuiText::Adpcm::pcmOffset, .isReset = true });
    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + PrKey::Post::Adpcm::pcmRatio, .title = GuiText::Adpcm::pcmRatio, .isReset = true });

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::ar, .title = GuiText::Adpcm::Adsr::ar, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::dr, .title = GuiText::Adpcm::Adsr::dr, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::sl, .title = GuiText::Adpcm::Adsr::sl, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::rr, .title = GuiText::Adpcm::Adsr::rr, .isReset = true });
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .label = &presetNameCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMain({ .mainRect = mRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &modeSelector.label, .component = &modeSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });
    layoutMain({ .mainRect = mRect, .component = &loopButton });
    layoutMain({ .mainRect = mRect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
    layoutMain({ .mainRect = mRect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &panCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &panSlider.label, .component = &panSlider });
    layoutMainThreeComps({ .rect = mRect, .comp1 = &btnPanL, .comp2 = &btnPanC, .comp3 = &btnPanR, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &attackSlider.label, .component = &attackSlider });
    layoutMain({ .mainRect = mRect, .label = &decaySlider.label, .component = &decaySlider });
    layoutMain({ .mainRect = mRect, .label = &sustainSlider.label, .component = &sustainSlider });
    layoutMain({ .mainRect = mRect, .label = &releaseSlider.label, .component = &releaseSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
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
