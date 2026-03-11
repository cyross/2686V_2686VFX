#include "GuiRhythm.h"

#include <vector>

#include "../editor/PluginEditor.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/FileValues.h"

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

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    juce::Logger::getCurrentLogger()->writeToLog(fileName);
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
    loadButton.setup({ .parent = *this, .title = GuiText::File::load, .isReset = false });
    loadButton.addListener(&ctx.editor);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centred);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // パッド音声アンロード
    clearButton.setup({ .parent = *this, .title = GuiText::File::clear, .isReset = false });
    clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    clearButton.onClick = [this, index]
        {
            // 1. 特定のパッドをアンロード
            ctx.audioProcessor.unloadRhythmFile(index);

            // 2. ファイル名表示を更新
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::pcmOffset, .title = GuiText::Rhythm::Pad::pcmOffset, .isReset = true });
    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::pcmRatio, .title = GuiText::Rhythm::Pad::pcmRatio, .isReset = true });

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    panCat.setup({ .parent = *this, .title = GuiText::Category::pan });

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::oneShot, .title = GuiText::Rhythm::Pad::oneShot, .isReset = true });

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::note, .title = GuiText::Rhythm::Pad::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    modeSelector.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::mode, .title = GuiText::Rhythm::Pad::quality, .items = qualityItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::rate, .title = GuiText::Rhythm::Pad::rate, .items = rateItems, .isReset = true });

    // パンポット
    panSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::pan, .title = GuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);

    btnPanL.setup({ .parent = *this, .title = GuiText::Rhythm::Pad::Pan::l, .isReset = false });
    btnPanL.addListener(&ctx.editor);

    btnPanC.setup({ .parent = *this, .title = GuiText::Rhythm::Pad::Pan::c, .isReset = false });
    btnPanC.addListener(&ctx.editor);

    btnPanR.setup({ .parent = *this, .title = GuiText::Rhythm::Pad::Pan::r, .isReset = false });
    btnPanR.addListener(&ctx.editor);

    // Vol
    volSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::volume, .title = GuiText::Rhythm::Pad::vol, .isReset = true });

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    // RR
    rrSlider.setup({ .parent = *this, .id = padPrefix + PrKey::Post::Rhythm::Pad::rr, .title = GuiText::Rhythm::Pad::rr, .isReset = true });
    rrTo000Button.setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo000, .isReset = false, .isResized = false });
    rrTo000Button.onClick = [this] { rrSlider.setValue(0.00, juce::sendNotification); };
    rrTo003Button.setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo003, .isReset = false, .isResized = false });
    rrTo003Button.onClick = [this] { rrSlider.setValue(0.03, juce::sendNotification); };
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto padRect = content.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    padRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = padRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = padRect, .label = &modeSelector.label, .component = &modeSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRowRhythmPadPcmFile({ .rect = padRect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = padRect, .component = &oneShotButton, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &noteSlider.label, .component = &noteSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = padRect, .label = &panCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = padRect, .label = &panSlider.label, .component = &panSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({
        .rect = padRect,
        .comp1 = &btnPanL, .comp2 = &btnPanC, .comp3 = &btnPanR,
        .paddingBottom = GuiValue::Category::paddingTop,
        .compWidth = GuiValue::ParamGroup::RhythmPan::width,
        .compPaddingRight = GuiValue::ParamGroup::RhythmPan::paddingRight
    });
    layoutRow({ .rowRect = padRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = padRect, .label = &rrSlider.label, .component = &rrSlider, .paddingBottom = 0 });
    layoutRowTwoComps({ .rect = padRect, .comp1 = &rrTo000Button, .comp2 = &rrTo003Button, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
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

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });

	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Rhythm::level, .title = GuiText::Rhythm::vol, .isReset = true });

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

    layoutMain({ .mainRect = mRect, .label = &presetNameCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMain({ .mainRect = mRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

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

void GuiRhythm::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
