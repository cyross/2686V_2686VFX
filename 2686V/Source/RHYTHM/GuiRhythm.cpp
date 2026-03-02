#include "GuiRhythm.h"
#include "../editor/PluginEditor.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/FileValues.h"

#include "../gui/GuiHelpers.h"

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

void RhythmPadGui::setup(juce::Component &parent, int index, juce::String padName)
{
    parent.addAndMakeVisible(this);

    juce::String padPrefix = PrKey::Prefix::rhythm + PrKey::Innder::pad + juce::String(index);
    juce::String padTitle = "Pad " + juce::String(index + 1) + " (" + padName + ")";

    // メイングループ
    mainGroup.setup(*this, padTitle);

    // 音声ファイルロードボタン
    loadButton.setup({ .parent = *this, .title = "Load", .isReset = false });
    loadButton.addListener(&ctx.editor);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
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
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::oneShot, .title = GuiText::Group::Rhythm::Pad::Post::oneShot, .isReset = true });

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::note, .title = GuiText::Group::Rhythm::Pad::Post::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    modeSelector.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::mode, .title = GuiText::Group::Rhythm::Pad::Post::quality, .items = qualityItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::rate, .title = GuiText::Group::Rhythm::Pad::Post::rate, .items = rateItems, .isReset = true });

    // パンポット
    panSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::pan, .title = GuiText::Group::Rhythm::Pad::Post::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);

    btnPanL.setup({ .parent = *this, .title = "L", .isReset = false });
    btnPanL.addListener(&ctx.editor);

    btnPanC.setup({ .parent = *this, .title = "C", .isReset = false });
    btnPanC.addListener(&ctx.editor);

    btnPanR.setup({ .parent = *this, .title = "R", .isReset = false });
    btnPanR.addListener(&ctx.editor);

    // Vol
    volSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::volume, .title = GuiText::Group::Rhythm::Pad::Post::vol, .isReset = true });

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    // RR
    rrSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::rr, .title = GuiText::Group::Rhythm::Pad::Post::rr, .isReset = true });
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto padRect = content.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    padRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = padRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = padRect, .label = &modeSelector.label, .component = &modeSelector });
    layoutComponentsLtoRRow({ .rowRect = padRect, .label = &rateSelector.label, .component = &rateSelector });
    layoutComponentsLtoRMain({ .mainRect = padRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRhythmPadPcmFileRow({ .rect = padRect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });
    layoutComponentsLtoRRow({ .rowRect = padRect, .component = &oneShotButton });
    layoutComponentsLtoRRow({ .rowRect = padRect, .label = &noteSlider.label, .component = &noteSlider });
    layoutComponentsLtoRRow({ .rowRect = padRect, .label = &panSlider.label, .component = &panSlider });
    layoutComponentsLtoRRhythmPadPanRow({ .rect = padRect, .lBtn = &btnPanL, .cBtn = &btnPanC, .rBtn = &btnPanR, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = padRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = padRect, .label = &rrSlider.label, .component = &rrSlider, .paddingBottom = 0 });
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
    const juce::String code = PrKey::Prefix::rhythm;

    // パッド名定義
    const std::array<juce::String, 8> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });

	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Rhythm::level, .title = "Vol", .isReset = true });

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

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    auto topPadsArea = pageArea.removeFromTop(GuiValue::Rhythm::Pad::height);
    auto bottomPadsArea = pageArea.removeFromTop(GuiValue::Rhythm::Pad::height);

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
