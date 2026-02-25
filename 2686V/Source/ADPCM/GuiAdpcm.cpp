#include "GuiAdpcm.h"
#include "../processor/PluginProcessor.h"

#include "../core/GuiConstants.h"
#include "../core/LabelConstants.h"
#include "../core/OpConstants.h"

void GuiAdpcm::setup()
{
    const juce::String code = codeAdpcm;

    mainGroup.setup(*this, mGroupTitle);
    modeSelector.setup({ .parent = *this, .id = code + postMode, .title = "Quality", .items = qualityItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });

    // 出力レベル
	levelSlider.setup({ .parent = *this, .id = code + postLevel, .title = "Vol", .isReset = true });

    // パンポット設定
	panSlider.setup({ .parent = *this, .id = code + postPan, .title = "Pan", .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    addAndMakeVisible(btnPanL); btnPanL.setButtonText("L"); btnPanL.addListener(&ctx.editor);
    addAndMakeVisible(btnPanC); btnPanC.setButtonText("C"); btnPanC.addListener(&ctx.editor);
    addAndMakeVisible(btnPanR); btnPanR.setButtonText("R"); btnPanR.addListener(&ctx.editor);

    // ループトグルボタン
	loopButton.setup({ .parent = *this, .id = code + postLoop, .title = "Loop", .isReset = true });

	paramGroup.setup(*this, "ADPCM Parameters");

    // 音声ファイル読み込みボタン
	loadButton.setup({ .parent = *this, .title = "Load File", .isReset = false });
    loadButton.addListener(&ctx.editor);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = *this, .title = emptyFilename});
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
	clearButton.setup({ .parent = *this, .title = "Clear", .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.onClick = [this]
    {
        // 1. プロセッサーのアンロード関数を呼ぶ
        ctx.audioProcessor.unloadAdpcmFile();

        // 2. ラベル表示をクリア
        fileNameLabel.setText(emptyFilename, juce::dontSendNotification);
    };

    attackSlider.setup({ .parent = *this, .id = code + postAr, .title = mArLabel, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + postDr, .title = mDrLabel, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + postSl, .title = mSlLabel, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + postRr, .title = mRrLabel, .isReset = true });
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);
    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &modeSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &modeSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &rateSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &rateSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &levelSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &levelSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &loopButton, { MainRegButtonWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &panSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &panSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, {
        { &btnPanL, { MainRegPanChangeButtonWidth, MainRegPanPaddingHeight}},
        { &btnPanC, { MainRegPanChangeButtonWidth, MainRegPanPaddingHeight}},
        { &btnPanR, { MainRegPanChangeButtonWidth, 0}}
        });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &attackSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &attackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &decaySlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &decaySlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &sustainSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &sustainSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &releaseSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &releaseSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

    auto headerArea = pageArea.removeFromTop(HeaderHeight);

    paramGroup.setBounds(headerArea);

    auto inner = headerArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    inner.removeFromTop(TitlePaddingTop);

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