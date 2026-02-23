#include "GuiRhythm.h"
#include "../editor/PluginEditor.h"
#include "../const/FileConstants.h"
#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

void RhythmPadGui::setup(juce::Component &parent, int index, juce::String padName)
{
    // Prepare Items for ComboBoxes
    std::vector<SelectItem> qualityItems = {
        {.name = "0: Raw (32bit)", .value = 1 },
        {.name = "1: 24-bit PCM",  .value = 2 },
        {.name = "2: 16-bit PCM",  .value = 3 },
        {.name = "3: 8-bit PCM",   .value = 4 },
        {.name = "4: 5-bit PCM",   .value = 5 },
        {.name = "5: 4-bit PCM",   .value = 6 },
        {.name = "6: 4-bit ADPCM", .value = 7 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    parent.addAndMakeVisible(this);

    juce::String padPrefix = codeRhythm + codePad + juce::String(index);
    juce::String padTitle = "Pad " + juce::String(index + 1) + " (" + padName + ")";

    // メイングループ
    mainGroup.setup(*this, padTitle);

    // 音声ファイルロードボタン
    loadButton.setup({ .parent = *this, .title = "Load", .isReset = false });
    loadButton.addListener(&ctx.editor);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = *this, .title = emptyFilename });
    fileNameLabel.setJustificationType(juce::Justification::centred);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // パッド音声アンロード
    clearButton.setup({ .parent = *this, .title = "X", .isReset = false });
    clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    clearButton.onClick = [this, index]
        {
            // 1. 特定のパッドをアンロード
            ctx.audioProcessor.unloadRhythmFile(index);

            // 2. ファイル名表示を更新
            fileNameLabel.setText("(Empty)", juce::dontSendNotification);
        };

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = *this, .id = padPrefix + postOneShot, .title = "One Shot", .isReset = true });

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = *this, .id = padPrefix + postNote, .title = "Note", .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    modeSelector.setup({ .parent = *this, .id = padPrefix + postMode, .title = "Mode", .items = qualityItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = padPrefix + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    // パンポット
    panSlider.setup({ .parent = *this, .id = padPrefix + postPan, .title = "Pan", .isReset = true });
    panSlider.setRange(0.0f, 1.0f);

    btnPanL.setup({ .parent = *this, .title = "L", .isReset = false });
    btnPanL.addListener(&ctx.editor);

    btnPanC.setup({ .parent = *this, .title = "C", .isReset = false });
    btnPanC.addListener(&ctx.editor);

    btnPanR.setup({ .parent = *this, .title = "R", .isReset = false });
    btnPanR.addListener(&ctx.editor);

    // Vol
    volSlider.setup({ .parent = *this, .id = padPrefix + postVolume, .title = "Vol", .isReset = true });

    // RR
    rrSlider.setup({ .parent = *this, .id = padPrefix + postRr, .title = opRrLabel, .isReset = true });
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto padRect = content.reduced(GroupPaddingWidth, GroupPaddingHeight);

    padRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &loadButton, { OpRegLabelWidth, OpRegPaddingRight}}, { &fileNameLabel, { 150, 0}},{ &clearButton, { 30, 0}}, });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &oneShotButton, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &noteSlider.label, { OpRegLabelWidth, OpRegPaddingRight}}, { &noteSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &modeSelector.label, { OpRegLabelWidth, OpRegPaddingRight}}, { &modeSelector, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &rateSelector.label, { OpRegLabelWidth, OpRegPaddingRight}}, { &rateSelector, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &panSlider.label, { OpRegLabelWidth, OpRegPaddingRight}}, { &panSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, {
        { &btnPanL, { OpRegPanChangeButtonWidth, 0} },
        { &btnPanC, { OpRegPanChangeButtonWidth, 0} },
        { &btnPanR, { OpRegPanChangeButtonWidth, 0} }
        });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &volSlider.label, { OpRegLabelWidth, OpRegPaddingRight}}, { &volSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &rrSlider.label, { OpRegLabelWidth, OpRegPaddingRight}}, { &rrSlider, { OpRegValueWidth, 0}} });
}

void RhythmPadGui::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

bool RhythmPadGui::isThis(juce::Button* button)
{
    return button == &loadButton;
}

void GuiRhythm::setup()
{
    const juce::String code = codeRhythm;

    // パッド名定義
    const std::array<juce::String, 8> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, mGroupTitle);

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });
   
	levelSlider.setup({ .parent = *this, .id = code + postLevel, .title = "Vol", .isReset = true });

    // Setup 8 Pads
    for (int i = 0; i < 8; ++i)
    {
        pads[i].setup(*this, i, padNames[i]);
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

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &levelSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &levelSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

    auto topPadsArea = pageArea.removeFromTop(RhythmPadsAreaHeight);
    auto bottomPadsArea = pageArea.removeFromTop(RhythmPadsAreaHeight);

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
