#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrValues.h"
#include "../../Core/Const/FileValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
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

static std::vector<SelectItem> wtWsItems = {
    {.name = "0: Sine",          .value = 1 },
    {.name = "1: Triangle",      .value = 2 },
    {.name = "2: Saw Up",        .value = 3 },
    {.name = "3: Saw Down",      .value = 4 },
    {.name = "4: Square (50%)",  .value = 5 },
    {.name = "5: Pulse (25%)",   .value = 6 },
    {.name = "6: Pulse (12.5%)", .value = 7 },
    {.name = "7: Digital Noise", .value = 8 },
    {.name = "8: Custom(Draw)",  .value = 9 },
};

static std::vector<SelectItem> wtTsItems = {
    {.name = "0:  32 Samples",  .value = 1 },
    {.name = "1:  64 Samples",  .value = 2 },
    {.name = "2: 128 Samples",  .value = 3 },
    {.name = "3: 256 Samples",  .value = 4 },
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

void GuiWt::setup()
{
    const juce::String code = PrKey::Prefix::wt;
    int tabOrder = 1;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });

    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::level, .title = GuiText::Wt::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    // Waveform
    waveSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::wave, .title = GuiText::Wt::form, .items = wtWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);

    // Custom Wave Size
    sizeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::sampleSize, .title = GuiText::Wt::size, .items = wtTsItems, .isReset = true, .isResized = true });
    sizeSelector.setWantsKeyboardFocus(true);
    sizeSelector.setExplicitFocusOrder(++tabOrder);

    modCat.setup({ .parent = *this, .title = GuiText::Category::mod });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::enable, .title = GuiText::Wt::Mod::enable, .isReset = true, .isResized = true });
    modEnableButton.setWantsKeyboardFocus(true);
    modEnableButton.setExplicitFocusOrder(++tabOrder);

    modDepthSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::depth, .title = GuiText::Wt::Mod::depth, .isReset = true });
    modDepthSlider.setWantsKeyboardFocus(true);
    modDepthSlider.setExplicitFocusOrder(++tabOrder);

    modSpeedSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::speed, .title = GuiText::Wt::Mod::speed, .isReset = true });
    modSpeedSlider.setWantsKeyboardFocus(true);
    modSpeedSlider.setExplicitFocusOrder(++tabOrder);

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    adsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setup({ .parent = *this, .title = GuiText::Category::pitchAdsr });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::pitchAdsr + PrKey::Post::bypass, .title = GuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::ar, .title = GuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::dr , .title = GuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::rr, .title = GuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::stl, .title = GuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::atl, .title = GuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::ssl, .title = GuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::rll, .title = GuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setup({ .parent = *this, .title = GuiText::Category::detune });

    dt1.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Op::dt2, .title = GuiText::Fm::Op::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    // Master Volume
	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    // Custom Wave Group
	customWaveGroup.setup(*this, GuiText::Group::wtCustom);

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom64 });
    customSliders128.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom128 });
    customSliders256.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom256 });

	customWaveResetTo0Btn.setup({ .parent = *this, .title = GuiText::Wt::Custom::to0, .bgColor = GuiColor::WaveformContainer::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.setWantsKeyboardFocus(true);
    customWaveResetTo0Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0.0f);
        customSliders64.setAllValues(0.0f);
        customSliders128.setAllValues(0.0f);
        customSliders256.setAllValues(0.0f);
        resized();
    };

    customWaveResetTo1Btn.setup({ .parent = *this, .title = GuiText::Wt::Custom::to1, .bgColor = GuiColor::WaveformContainer::ResetBtn::To1, .isReset = false, .isResized = false });
    customWaveResetTo1Btn.setWantsKeyboardFocus(true);
    customWaveResetTo1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo1Btn.onClick = [this] {
        customSliders32.setAllValues(1.0f);
        customSliders64.setAllValues(1.0f);
        customSliders128.setAllValues(1.0f);
        customSliders256.setAllValues(1.0f);
        resized();
        };

    customWaveResetToM1Btn.setup({ .parent = *this, .title = GuiText::Wt::Custom::toM1, .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1, .isReset = false, .isResized = false });
    customWaveResetToM1Btn.setWantsKeyboardFocus(true);
    customWaveResetToM1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetToM1Btn.onClick = [this] {
        customSliders32.setAllValues(-1.0f);
        customSliders64.setAllValues(-1.0f);
        customSliders128.setAllValues(-1.0f);
        customSliders256.setAllValues(-1.0f);
        resized();
        };

    customWaveSmoothBtn.setup({ .parent = *this, .title = GuiText::Wt::Custom::smooth, .bgColor = juce::Colours::darkcyan, .isReset = false, .isResized = false });
    customWaveSmoothBtn.setWantsKeyboardFocus(true);
    customWaveSmoothBtn.setExplicitFocusOrder(++tabOrder);
    customWaveSmoothBtn.onClick = [this] {
        // 現在選択されている波形サイズに応じてスムージングを実行
        int sizeId = sizeSelector.getSelectedId();
        if (sizeId == 1) customSliders32.applySmoothing();
        else if (sizeId == 2) customSliders64.applySmoothing();
        else if (sizeId == 3) customSliders128.applySmoothing();
        else if (sizeId == 4) customSliders256.applySmoothing();
        };

    waveFileCat.setup({ .parent = *this, .title = GuiText::Category::waveFile });

    customWaveImportBtn.setup({ .parent = *this, .title = GuiText::Wt::fileImport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveImportBtn.setWantsKeyboardFocus(true);
    customWaveImportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveImportBtn.onClick = [this] { importWavetable(); };

    customWaveExportBtn.setup({ .parent = *this, .title = GuiText::Wt::fileExport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveExportBtn.setWantsKeyboardFocus(true);
    customWaveExportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveExportBtn.onClick = [this] { exportWavetable(); };
}

void GuiWt::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMainCategory({ .mainRect = mRect, .label = &qualityCat });
    layoutMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, });
    layoutMainCategory({ .mainRect = mRect, .label = &mainCat });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider, });
    layoutMain({ .mainRect = mRect, .label = &waveSelector.label, .component = &waveSelector });
    layoutMain({ .mainRect = mRect, .label = &sizeSelector.label, .component = &sizeSelector, });
    layoutMainCategory({ .mainRect = mRect, .label = &modCat });
    layoutMain({ .mainRect = mRect, .component = &modEnableButton });
    layoutMain({ .mainRect = mRect, .label = &modDepthSlider.label, .component = &modDepthSlider });
    layoutMain({ .mainRect = mRect, .label = &modSpeedSlider.label, .component = &modSpeedSlider, });

    layoutMainCategory({ .mainRect = mRect, .label = &adsrCat });
    layoutMain({ .mainRect = mRect, .component = &adsrBypassButton });
    layoutMain({ .mainRect = mRect, .label = &attackSlider.label, .component = &attackSlider });
    layoutMain({ .mainRect = mRect, .label = &decaySlider.label, .component = &decaySlider });
    layoutMain({ .mainRect = mRect, .label = &sustainSlider.label, .component = &sustainSlider });
    layoutMain({ .mainRect = mRect, .label = &releaseSlider.label, .component = &releaseSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &pitchAdsrCat });
    layoutMain({ .mainRect = mRect, .component = &pitchAdsrBypassButton });
    layoutMain({ .mainRect = mRect, .label = &pitchAttackSlider.label, .component = &pitchAttackSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchDecaySlider.label, .component = &pitchDecaySlider });
    layoutMain({ .mainRect = mRect, .label = &pitchReleaseSlider.label, .component = &pitchReleaseSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchStartLevelSlider.label, .component = &pitchStartLevelSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchAttackLevelSlider.label, .component = &pitchAttackLevelSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchSustainLevelSlider.label, .component = &pitchSustainLevelSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchReleaseLevelSlider.label, .component = &pitchReleaseLevelSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &detuneCat });
    layoutMain({ .mainRect = mRect, .label = &dt1.label, .component = &dt1 });
    layoutMain({ .mainRect = mRect, .label = &dt2.label, .component = &dt2 });

    layoutMainCategory({ .mainRect = mRect, .label = &waveFileCat });
    layoutMain({ .mainRect = mRect, .component = &customWaveImportBtn });
    layoutMain({ .mainRect = mRect, .component = &customWaveExportBtn });

    layoutMainCategory({ .mainRect = mRect, .label = &monoPolyCat });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    bool isMod = modEnableButton.getToggleState();
    modDepthSlider.setEnabled(isMod);
    modDepthSlider.label.setEnabled(isMod);
    modSpeedSlider.setEnabled(isMod);
    modSpeedSlider.label.setEnabled(isMod);

    // 波形がカスタム以外の時は波形精度選択を Disabled に
    bool isCustomWave = (waveSelector.getSelectedId() == 9);

    sizeSelector.setEnabled(isCustomWave);
    sizeSelector.label.setEnabled(isCustomWave);
    customSliders32.setEnabled(isCustomWave);
    customSliders64.setEnabled(isCustomWave);
    customSliders128.setEnabled(isCustomWave);
    customSliders256.setEnabled(isCustomWave);
    customWaveResetTo0Btn.setEnabled(isCustomWave);
    customWaveResetTo1Btn.setEnabled(isCustomWave);
    customWaveResetToM1Btn.setEnabled(isCustomWave);
    customWaveSmoothBtn.setEnabled(isCustomWave);

    // Custom Wave
    // Custom Mode Layout
    // 中央に32/64本のスライダーを配置
    // 波形選択が Custom 以外の時は Disabled 表示
    auto rightArea = pageArea.removeFromLeft(GuiValue::Wt::RightWidth).removeFromTop(GuiValue::Wt::RightHeight);

    customWaveGroup.setBounds(rightArea);

    auto cwRect = customWaveGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    cwRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    auto containerArea = cwRect.removeFromTop(GuiValue::Wt::Custom::Slider::Height);

    customSliders32.setBounds(containerArea);
    customSliders64.setBounds(containerArea);
    customSliders128.setBounds(containerArea);
    customSliders256.setBounds(containerArea);

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    int waveSize = 0;

    switch (sizeSelector.getSelectedId())
    {
    case 1:
        waveSize = 32;
        break;
    case 2:
        waveSize = 64;
        break;
    case 3:
        waveSize = 128;
        break;
    case 4:
        waveSize = 256;
        break;
    }

    customWaveGroup.setEnabled(isCustomWave);

    customSliders32.setVisible(false);
    customSliders32.setCustomEnabled(false);
    customSliders64.setVisible(false);
    customSliders64.setCustomEnabled(false);
    customSliders128.setVisible(false);
    customSliders128.setCustomEnabled(false);
    customSliders256.setVisible(false);
    customSliders256.setCustomEnabled(false);

    if (isCustomWave) {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
            customSliders32.setCustomEnabled(true);
        }
        else if (waveSize == 64) {
            customSliders64.setVisible(true);
            customSliders64.setCustomEnabled(true);
        }
        else if (waveSize == 128) {
            customSliders128.setVisible(true);
            customSliders128.setCustomEnabled(true);
        }
        else {
            customSliders256.setVisible(true);
            customSliders256.setCustomEnabled(true);
        }
    }
    else {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
        }
        else if (waveSize == 64) {
            customSliders64.setVisible(true);
        }
        else if (waveSize == 128) {
            customSliders128.setVisible(true);
        }
        else {
            customSliders256.setVisible(true);
        }
    }

    cwRect.removeFromTop(GuiValue::Wt::Custom::ResetBtn::Padding::Top);

    auto smoothArea = cwRect.removeFromTop(14).reduced(2, 0);
    customWaveSmoothBtn.setBounds(smoothArea);

    cwRect.removeFromTop(GuiValue::Wt::Custom::ResetBtn::Padding::Top);
    layoutRowWtWaveValueUpdate({ .rect = cwRect, .resetTo0Btn = &customWaveResetTo0Btn, .resetTo1Btn = &customWaveResetTo1Btn, .resetToM1Btn = &customWaveResetToM1Btn });
}

void GuiWt::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

// ==============================================================================
// Import / Export Logic
// ==============================================================================
void GuiWt::importWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importWavetableFile, defaultDir, Io::ExtensionGlob::wavetable);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                if (lines.size() == 0) return;

                // 1行目のサンプル数を取得
                int sampleCount = lines[0].trim().getIntValue();

                // サンプル数の検証
                if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                        "Invalid WT File", "Sample count must be 32, 64, 128, or 256.");
                    return;
                }

                // デフォルト0.0で配列を初期化（足りない部分は0.0で埋まる）
                std::vector<float> values(sampleCount, 0.0f);

                // 2行目以降の値を読み込み、-1.0 ~ 1.0 にクランプして格納
                for (int i = 0; i < sampleCount; ++i) {
                    if (i + 1 < lines.size()) {
                        float val = lines[i + 1].getFloatValue();
                        values[i] = std::clamp(val, -1.0f, 1.0f);
                    }
                }

                // --- UIの更新 ---
                // 波形を「8: Custom(Draw)」に変更 (ID: 9)
                waveSelector.setSelectedId(9, juce::sendNotification);

                // サンプルサイズを選択
                int sizeId = 1;
                if (sampleCount == 32) sizeId = 1;
                else if (sampleCount == 64) sizeId = 2;
                else if (sampleCount == 128) sizeId = 3;
                else if (sampleCount == 256) sizeId = 4;
                sizeSelector.setSelectedId(sizeId, juce::sendNotification);

                // --- 値をAPVTS(スライダー)に反映 ---
                if (sampleCount == 32) customSliders32.setValues(values);
                else if (sampleCount == 64) customSliders64.setValues(values);
                else if (sampleCount == 128) customSliders128.setValues(values);
                else if (sampleCount == 256) customSliders256.setValues(values);
            }
        });
}

void GuiWt::exportWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportWavetableFile, defaultDir.getChildFile("custom_wave.wt"), Io::ExtensionGlob::wavetable);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                // 現在のサイズIDを取得
                int sizeId = sizeSelector.getSelectedId();
                std::vector<float> values;

                if (sizeId == 1) values = customSliders32.getValues();
                else if (sizeId == 2) values = customSliders64.getValues();
                else if (sizeId == 3) values = customSliders128.getValues();
                else if (sizeId == 4) values = customSliders256.getValues();

                if (values.empty()) return;

                // 1行目にサンプル数
                juce::String content = juce::String(values.size()) + "\n";

                // 2行目以降に値を書き込む
                for (float v : values) {
                    content += juce::String(v, 6) + "\n"; // 小数点以下6桁まで保存
                }

                file.replaceWithText(content);
            }
        });
}
