#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Rhythm/Keys.h"
#include "../../Processor/Rhythm/Values.h"
#include "../../Core/Const/FileValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
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
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 22.05kHz", .value = 6 },
    {.name = " 7: 16kHz",    .value = 7 },
    {.name = " 8: 12kHz",    .value = 8 },
    {.name = " 9: 11kHz",    .value = 9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
};

void RhythmPadGui::updatePadFileName(const juce::String& fileName)
{
    juce::Logger::getCurrentLogger()->writeToLog(fileName);
    fileNameLabel.setText(fileName, juce::dontSendNotification);
}

void RhythmPadGui::setup(juce::Component &parent, int index, juce::String padName, int& tabOrder)
{
    parent.addAndMakeVisible(this);

    juce::String padPrefix = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(index);
    juce::String padTitle = "Pad " + juce::String(index + 1) + " (" + padName + ")";

    // メイングループ
    mainGroup.setup(*this, padTitle);

    qualityCat.setup({ .parent = *this, .title = RhythmGuiText::Category::quality });

    modeSelector.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::mode, .title = RhythmGuiText::Rhythm::Pad::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::rate, .title = RhythmGuiText::Rhythm::Pad::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    mainCat.setup({ .parent = *this, .title = RhythmGuiText::Category::m });

    // 音声ファイルロードボタン
    loadButton.setup({ .parent = *this, .title = RhythmGuiText::File::load, .isReset = false });
    loadButton.addListener(&ctx.editor);
    loadButton.setWantsKeyboardFocus(true);
    loadButton.setExplicitFocusOrder(++tabOrder);

    // ロードしている音声ファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centred);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // パッド音声アンロード
    clearButton.setup({ .parent = *this, .title = RhythmGuiText::File::clear, .isReset = false });
    clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this, index]
        {
            // 1. 特定のパッドをアンロード
            ctx.audioProcessor.unloadRhythmFile(index);

            // 2. ファイル名表示を更新
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pcmOffset, .title = RhythmGuiText::Rhythm::Pad::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pcmRatio, .title = RhythmGuiText::Rhythm::Pad::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // Vol
    volSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::volume, .title = RhythmGuiText::Rhythm::Pad::vol, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    // ワンショット機能トグル
    oneShotButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::oneShot, .title = RhythmGuiText::Rhythm::Pad::oneShot, .isReset = true });
    oneShotButton.setWantsKeyboardFocus(true);
    oneShotButton.setExplicitFocusOrder(++tabOrder);

    // 割り当てキーノート番号
    noteSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::note, .title = RhythmGuiText::Rhythm::Pad::note, .isReset = true });
    noteSlider.setRange(0, 127, 1);
    noteSlider.setWantsKeyboardFocus(true);
    noteSlider.setExplicitFocusOrder(++tabOrder);
    noteSlider.textFromValueFunction = [](double value) {
        return getNoteName((int)value);
        };
    noteSlider.updateText();

    panCat.setup({ .parent = *this, .title = RhythmGuiText::Category::pan });

    // パンポット
    panSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pan, .title = RhythmGuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);
    panSlider.setRange(0.0f, 1.0f);

    btnPanL.setup({ .parent = *this, .title = RhythmGuiText::Rhythm::Pad::Pan::l, .isReset = false });
    btnPanL.setWantsKeyboardFocus(true);
    btnPanL.setExplicitFocusOrder(++tabOrder);
    btnPanL.addListener(&ctx.editor);

    btnPanC.setup({ .parent = *this, .title = RhythmGuiText::Rhythm::Pad::Pan::c, .isReset = false });
    btnPanC.setWantsKeyboardFocus(true);
    btnPanC.setExplicitFocusOrder(++tabOrder);
    btnPanC.addListener(&ctx.editor);

    btnPanR.setup({ .parent = *this, .title = RhythmGuiText::Rhythm::Pad::Pan::r, .isReset = false });
    btnPanR.setWantsKeyboardFocus(true);
    btnPanR.setExplicitFocusOrder(++tabOrder);
    btnPanR.addListener(&ctx.editor);

    adsrCat.setup({ .parent = *this, .title = RhythmGuiText::Category::adsr });

    // RR
    rrSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::rr, .title = RhythmGuiText::Rhythm::Pad::rr, .isReset = true });
    rrSlider.setWantsKeyboardFocus(true);
    rrSlider.setExplicitFocusOrder(++tabOrder);

    rrTo000Button.setup(GuiTextButton::Config{ .parent = *this, .title = RhythmGuiText::Fm::Op::ArTo000, .isReset = false, .isResized = false });
    rrTo000Button.setWantsKeyboardFocus(true);
    rrTo000Button.setExplicitFocusOrder(++tabOrder);
    rrTo000Button.onClick = [this] { rrSlider.setValue(0.00, juce::sendNotification); };

    rrTo003Button.setup(GuiTextButton::Config{ .parent = *this, .title = RhythmGuiText::Fm::Op::ArTo003, .isReset = false, .isResized = false });
    rrTo003Button.setWantsKeyboardFocus(true);
    rrTo003Button.setExplicitFocusOrder(++tabOrder);
    rrTo003Button.onClick = [this] { rrSlider.setValue(0.03, juce::sendNotification); };
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto padRect = content.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);

    padRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = padRect, .label = &qualityCat });
    layoutRow({ .rowRect = padRect, .label = &modeSelector.label, .component = &modeSelector });
    layoutRow({ .rowRect = padRect, .label = &rateSelector.label, .component = &rateSelector, });
    layoutRowCategory({ .rowRect = padRect, .label = &mainCat });
    layoutRowRhythmPadPcmFile({ .rect = padRect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton });
    layoutRow({ .rowRect = padRect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
    layoutRow({ .rowRect = padRect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });
    layoutRow({ .rowRect = padRect, .component = &oneShotButton });
    layoutRow({ .rowRect = padRect, .label = &noteSlider.label, .component = &noteSlider, });
    layoutRowCategory({ .rowRect = padRect, .label = &panCat });
    layoutRow({ .rowRect = padRect, .label = &panSlider.label, .component = &panSlider });
    layoutRowThreeComps({
        .rect = padRect,
        .comp1 = &btnPanL, .comp2 = &btnPanC, .comp3 = &btnPanR,
        .compWidth = RhythmGuiValue::ParamGroup::RhythmPan::width,
        .compPaddingRight = RhythmGuiValue::ParamGroup::RhythmPan::paddingRight
    });
    layoutRowCategory({ .rowRect = padRect, .label = &adsrCat });
    layoutRow({ .rowRect = padRect, .label = &rrSlider.label, .component = &rrSlider, .paddingBottom = 0 });
    layoutRowTwoComps({ .rect = padRect, .comp1 = &rrTo000Button, .comp2 = &rrTo003Button });
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
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;

    // パッド名定義
    const std::array<juce::String, 8> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = RhythmGuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    mainCat.setup({ .parent = *this, .title = RhythmGuiText::Category::m });

    levelSlider.setup({ .parent = *this, .id = code + RhythmPrKey::level, .title = RhythmGuiText::Rhythm::vol, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = RhythmGuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = RhythmPrKey::masterVol, .title = RhythmGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    // Setup 8 Pads
    for (int i = 0; i < 8; ++i)
    {
        pads[i].setup(*this, i, padNames[i], tabOrder);
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

    auto mainArea = pageArea.removeFromLeft(RhythmGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);
    mRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = RhythmGuiValue::PresetName::paddingBottom });
    layoutMainCategory({ .mainRect = mRect, .label = &mainCat });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    auto topPadsArea = pageArea.removeFromTop(RhythmGuiValue::Rhythm::Pad::height);
    auto bottomPadsArea = pageArea.removeFromTop(RhythmGuiValue::Rhythm::Pad::height);

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

void GuiRhythm::initParams()
{
    this->ctx.audioProcessor.initParams("RHYTHM_");
    for (int i = 0; i < 8; i++)
    {
        this->ctx.audioProcessor.unloadRhythmFile(i);
        updatePadFileName(i, Io::empty);
    }
}
