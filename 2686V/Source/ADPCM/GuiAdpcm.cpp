#include "GuiAdpcm.h"
#include "../processor/PluginProcessor.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

void GuiAdpcm::setup()
{
    const juce::String code = PrKey::Prefix::adpcm;

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    modeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::mode, .title = GuiText::Group::Adpcm::Post::quality, .items = qualityItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::rate, .title = GuiText::Group::Adpcm::Post::rate, .items = rateItems, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // 出力レベル
	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::level, .title = "Vol", .isReset = true });

    // パンポット設定
	panSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::pan, .title = "Pan", .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    addAndMakeVisible(btnPanL); btnPanL.setButtonText("L"); btnPanL.addListener(&ctx.editor);
    addAndMakeVisible(btnPanC); btnPanC.setButtonText("C"); btnPanC.addListener(&ctx.editor);
    addAndMakeVisible(btnPanR); btnPanR.setButtonText("R"); btnPanR.addListener(&ctx.editor);

    // ループトグルボタン
	loopButton.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::loop, .title = "Loop", .isReset = true });

	paramGroup.setup(*this, "ADPCM Parameters");

    // 音声ファイル読み込みボタン
	loadButton.setup({ .parent = *this, .title = "Load File", .isReset = false });
    loadButton.addListener(&ctx.editor);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
	clearButton.setup({ .parent = *this, .title = "Clear", .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.onClick = [this]
    {
        // 1. プロセッサーのアンロード関数を呼ぶ
        ctx.audioProcessor.unloadAdpcmFile();

        // 2. ラベル表示をクリア
        fileNameLabel.setText(Io::empty, juce::dontSendNotification);
    };

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::ar, .title = GuiText::Group::Adpcm::Post::Adsr::ar, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::dr, .title = GuiText::Group::Adpcm::Post::Adsr::dr, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::sl, .title = GuiText::Group::Adpcm::Post::Adsr::sl, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adpcm::Adsr::rr, .title = GuiText::Group::Adpcm::Post::Adsr::rr, .isReset = true });
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &modeSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &modeSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &rateSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &rateSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &levelSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &levelSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &loopButton, { GuiValue::MainGroup::Button::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &panSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &panSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &btnPanL, { GuiValue::MainGroup::Button::Pan::width, GuiValue::MainGroup::Button::Pan::paddingHeight}},
        { &btnPanC, { GuiValue::MainGroup::Button::Pan::width, GuiValue::MainGroup::Button::Pan::paddingHeight}},
        { &btnPanR, { GuiValue::MainGroup::Button::Pan::width, 0}}
        });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &attackSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &attackSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &decaySlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &decaySlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &sustainSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &sustainSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &releaseSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &releaseSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, { { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} } });

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