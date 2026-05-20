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
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    parent.addAndMakeVisible(this);

    juce::String padPrefix = RhythmPrKey::prefix + RhythmPrKey::pad + juce::String(index);
    juce::String padTitle = RhythmGuiText::Group::padPrefix +  " " + juce::String(index + 1) + " (" + padName + ")";

    // メイングループ
    mainGroup.setup(*this, padTitle);

    qualityCat.setup({ .parent = *this, .title = RhythmGuiText::Category::visibleQuality, .invisibleTitle = RhythmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    modeSelector.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::mode, .title = RhythmGuiText::Rhythm::Pad::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::rate, .title = RhythmGuiText::Rhythm::Pad::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

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

    panCat.setup({ .parent = *this, .title = RhythmGuiText::Category::visiblePan, .invisibleTitle = RhythmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    // パンポット
    panSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::pan, .title = RhythmGuiText::Rhythm::Pad::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);
    panSlider.setRange(0.0f, 1.0f);

    setupPanBtn(panToLBtn, RhythmGuiText::Rhythm::Pad::Pan::l, tabOrder);
    setupPanBtn(panToCBtn, RhythmGuiText::Rhythm::Pad::Pan::c, tabOrder);
    setupPanBtn(panToRBtn, RhythmGuiText::Rhythm::Pad::Pan::r, tabOrder);

    adsrCat.setup({ .parent = *this, .title = RhythmGuiText::Category::visibleAdsr, .invisibleTitle = RhythmGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    adsrBypassButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::adsr + RhythmPrKey::bypass, .title = RhythmGuiText::Rhythm::Pad::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::stl, .title = RhythmGuiText::Rhythm::Pad::Adsr::stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::ar, .title = RhythmGuiText::Rhythm::Pad::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::dr , .title = RhythmGuiText::Rhythm::Pad::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::sl, .title = RhythmGuiText::Rhythm::Pad::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::Adsr::rr, .title = RhythmGuiText::Rhythm::Pad::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setup({ .parent = *this, .title = RhythmGuiText::Category::visiblePitchAdsr, .invisibleTitle = RhythmGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::pitchAdsr + RhythmPrKey::bypass, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::ar, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::dr , .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::rr, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::stl, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::atl, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::ssl, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::PitchAdsr::rll, .title = RhythmGuiText::Rhythm::Pad::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwEnvCat.setup({ .parent = *this, .title = RhythmGuiText::Category::visibleSwEnv, .invisibleTitle = RhythmGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

    ssgSwEnvBypassButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::ssgSwEnv + RhythmPrKey::bypass, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::bypass, .isReset = true });
    ssgSwEnvBypassButton.setWantsKeyboardFocus(true);
    ssgSwEnvBypassButton.setExplicitFocusOrder(++tabOrder);

    ssgSwStepsSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::steps, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::steps, .isReset = true });
    ssgSwStepsSlider.setWantsKeyboardFocus(true);
    ssgSwStepsSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwEnvLoopButton.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::loop, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::loop, .isReset = true });
    ssgSwEnvLoopButton.setWantsKeyboardFocus(true);
    ssgSwEnvLoopButton.setExplicitFocusOrder(++tabOrder);

    ssgSwLoopToSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::loopTo, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::loopTo, .isReset = true });
    ssgSwLoopToSlider.setWantsKeyboardFocus(true);
    ssgSwLoopToSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwLoopCountSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::loopCount, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::loopCount, .isReset = true });
    ssgSwLoopCountSlider.setWantsKeyboardFocus(true);
    ssgSwLoopCountSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwStartLevelSlider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::stl, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::stl, .isReset = true });
    ssgSwStartLevelSlider.setWantsKeyboardFocus(true);
    ssgSwStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwR1Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::r1, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::r1, .isReset = true });
    ssgSwR1Slider.setWantsKeyboardFocus(true);
    ssgSwR1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL1Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::l1, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::l1, .isReset = true });
    ssgSwL1Slider.setWantsKeyboardFocus(true);
    ssgSwL1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR2Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::r2, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::r2, .isReset = true });
    ssgSwR2Slider.setWantsKeyboardFocus(true);
    ssgSwR2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL2Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::l2, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::l2, .isReset = true });
    ssgSwL2Slider.setWantsKeyboardFocus(true);
    ssgSwL2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR3Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::r3, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::r3, .isReset = true });
    ssgSwR3Slider.setWantsKeyboardFocus(true);
    ssgSwR3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL3Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::l3, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::l3, .isReset = true });
    ssgSwL3Slider.setWantsKeyboardFocus(true);
    ssgSwL3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR4Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::r4, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::r4, .isReset = true });
    ssgSwR4Slider.setWantsKeyboardFocus(true);
    ssgSwR4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL4Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::l4, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::l4, .isReset = true });
    ssgSwL4Slider.setWantsKeyboardFocus(true);
    ssgSwL4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR5Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::r5, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::r5, .isReset = true });
    ssgSwR5Slider.setWantsKeyboardFocus(true);
    ssgSwR5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL5Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::l5, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::l5, .isReset = true });
    ssgSwL5Slider.setWantsKeyboardFocus(true);
    ssgSwL5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR6Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::r6, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::r6, .isReset = true });
    ssgSwR6Slider.setWantsKeyboardFocus(true);
    ssgSwR6Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL6Slider.setup({ .parent = *this, .id = padPrefix + RhythmPrKey::Pad::SsgSwEnv::l6, .title = RhythmGuiText::Rhythm::Pad::SsgSwEnv::l6, .isReset = true });
    ssgSwL6Slider.setWantsKeyboardFocus(true);
    ssgSwL6Slider.setExplicitFocusOrder(++tabOrder);
}

void RhythmPadGui::layout(juce::Rectangle<int> content)
{
    mainGroup.setBounds(content);

    auto padRect = content.reduced(RhythmGuiValue::Group::Padding::width, RhythmGuiValue::Group::Padding::height);

    padRect.removeFromTop(RhythmGuiValue::Group::TitlePaddingTop);

    layoutRowRhythmPadPcmFile({ .rect = padRect, .loadBtn = &loadButton, .filenameLabel = &fileNameLabel, .clearBtn = &clearButton });
    layoutRow({ .rowRect = padRect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
    layoutRow({ .rowRect = padRect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
    layoutRow({ .rowRect = padRect, .label = &volSlider.label, .component = &volSlider });
    layoutRow({ .rowRect = padRect, .component = &oneShotButton });
    layoutRow({ .rowRect = padRect, .label = &noteSlider.label, .component = &noteSlider, });

    layoutPanCat(padRect);

    layoutAdsrCat(padRect);

	layoutPitchEnvCat(padRect);

	layoutSsgSwEnvCat(padRect);

    layoutQualityCat(padRect);
}

void RhythmPadGui::removeLoadButtonListener(AudioPlugin2686VEditor* editor)
{
    loadButton.removeListener(editor);
}

bool RhythmPadGui::isThis(juce::Button* button)
{
    return button == &loadButton;
}

void RhythmPadGui::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .label = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutRow({ .rowRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void RhythmPadGui::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutRowThreeComps({
            .rect = rect,
            .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn,
            .compWidth = RhythmGuiValue::ParamGroup::RhythmPan::width,
            .compPaddingRight = RhythmGuiValue::ParamGroup::RhythmPan::paddingRight
            });
    }
}

void RhythmPadGui::layoutAdsrCat(juce::Rectangle<int>& rect)
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


void RhythmPadGui::layoutPitchEnvCat(juce::Rectangle<int>& rect)
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

void RhythmPadGui::layoutSsgSwEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &ssgSwEnvCat });

    bool visible = ssgSwEnvCat.isDetailVisible();

    ssgSwEnvBypassButton.setVisible(visible);
    ssgSwStepsSlider.setVisibleWithLabel(visible);
    ssgSwEnvLoopButton.setVisible(visible);
    ssgSwLoopToSlider.setVisibleWithLabel(visible);
    ssgSwLoopCountSlider.setVisibleWithLabel(visible);
    ssgSwStartLevelSlider.setVisibleWithLabel(visible);
    ssgSwR1Slider.setVisibleWithLabel(visible);
    ssgSwL1Slider.setVisibleWithLabel(visible);
    ssgSwR2Slider.setVisibleWithLabel(visible);
    ssgSwL2Slider.setVisibleWithLabel(visible);
    ssgSwR3Slider.setVisibleWithLabel(visible);
    ssgSwL3Slider.setVisibleWithLabel(visible);
    ssgSwR4Slider.setVisibleWithLabel(visible);
    ssgSwL4Slider.setVisibleWithLabel(visible);
    ssgSwR5Slider.setVisibleWithLabel(visible);
    ssgSwL5Slider.setVisibleWithLabel(visible);
    ssgSwR6Slider.setVisibleWithLabel(visible);
    ssgSwL6Slider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &ssgSwEnvBypassButton });
        layoutMain({ .mainRect = rect, .label = &ssgSwStepsSlider.label, .component = &ssgSwStepsSlider });
        layoutMain({ .mainRect = rect, .component = &ssgSwEnvLoopButton });
        layoutMain({ .mainRect = rect, .label = &ssgSwLoopToSlider.label, .component = &ssgSwLoopToSlider });
        layoutMain({ .mainRect = rect, .label = &ssgSwLoopCountSlider.label, .component = &ssgSwLoopCountSlider });
        layoutMain({ .mainRect = rect, .label = &ssgSwStartLevelSlider.label, .component = &ssgSwStartLevelSlider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR1Slider.label, .component = &ssgSwR1Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL1Slider.label, .component = &ssgSwL1Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR2Slider.label, .component = &ssgSwR2Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL2Slider.label, .component = &ssgSwL2Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR3Slider.label, .component = &ssgSwR3Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL3Slider.label, .component = &ssgSwL3Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR4Slider.label, .component = &ssgSwR4Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL4Slider.label, .component = &ssgSwL4Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR5Slider.label, .component = &ssgSwR5Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL5Slider.label, .component = &ssgSwL5Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR6Slider.label, .component = &ssgSwR6Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL6Slider.label, .component = &ssgSwL6Slider });
    }
}

void GuiRhythm::setup()
{
    const juce::String code = RhythmPrKey::prefix;
    int tabOrder = 1;

    // パッド名定義
    const std::array<juce::String, 8> padNames = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    mainGroup.setup(*this, RhythmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    levelSlider.setup({ .parent = *this, .id = code + RhythmPrKey::level, .title = RhythmGuiText::Rhythm::vol, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = RhythmGuiText::Category::visibleMvol, .invisibleTitle = RhythmGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

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

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutMvolCat(mRect);

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

void GuiRhythm::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}
