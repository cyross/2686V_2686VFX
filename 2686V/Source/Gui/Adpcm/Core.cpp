#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Adpcm/Keys.h"
#include "../../Processor/Adpcm/Values.h"

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
    {.name = " 1: 96kHz",    .value =  1 },
    {.name = " 2: 55.5kHz",  .value =  2 },
    {.name = " 3: 49.7kHz",  .value =  3 },
    {.name = " 4: 48kHz",    .value =  4 },
    {.name = " 5: 44.1kHz",  .value =  5 },
    {.name = " 6: 22.05kHz", .value =  6 },
    {.name = " 7: 16kHz",    .value =  7 },
    {.name = " 8: 12kHz",    .value =  8 },
    {.name = " 9: 11kHz",    .value =  9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
};

static std::vector<SelectItem> multems = {
    {.name = " 0: x  0.5",    .value = 1 },
    {.name = " 1: x  0.891",  .value = 2 },
    {.name = " 2: x  1",      .value = 3 },
    {.name = " 3: x  1.414",  .value = 4 },
    {.name = " 4: x  1.498",  .value = 5 },
    {.name = " 5: x  1.581",  .value = 6 },
    {.name = " 6: x  1.781",  .value = 7 },
    {.name = " 7: x  2",      .value = 8 },
    {.name = " 8: x  3",      .value = 9 },
    {.name = " 9: x  4",      .value = 10 },
    {.name = "10: x  5",      .value = 11 },
    {.name = "11: x  6",      .value = 12 },
    {.name = "12: x  7",      .value = 13 },
    {.name = "13: x  8",      .value = 14 },
    {.name = "14: x  9",      .value = 15 },
    {.name = "15: x 10",      .value = 16 },
    {.name = "16: x 11",      .value = 17 },
    {.name = "17: x 12",      .value = 18 },
    {.name = "18: x 13",      .value = 19 },
    {.name = "19: x 14",      .value = 20 },
    {.name = "20: x 15",      .value = 21 },
    {.name = "21: Use Ratio", .value = 22 }
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
void GuiAdpcm::setup()
{
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.addListener(&ctx.editor);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    const juce::String code = AdpcmPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, AdpcmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    qualityCat.setupHwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visibleQuality, .invisibleTitle = AdpcmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    modeSelector.setup({ .parent = *this, .id = code + AdpcmPrKey::mode, .title = AdpcmGuiText::Adpcm::quality, .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + AdpcmPrKey::rate, .title = AdpcmGuiText::Adpcm::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    // 出力レベル
	levelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::level, .title = AdpcmGuiText::Adpcm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    optionalCat.setupSwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visibleOptional, .invisibleTitle = AdpcmGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

    // ループトグルボタン
    loopButton.setup({ .parent = *this, .id = code + AdpcmPrKey::loop, .title = AdpcmGuiText::Adpcm::loop, .isReset = true });
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    pcmOffsetSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + AdpcmPrKey::pcmOffset, .title = AdpcmGuiText::Adpcm::pcmOffset, .isReset = true });
    pcmOffsetSlider.setWantsKeyboardFocus(true);
    pcmOffsetSlider.setExplicitFocusOrder(++tabOrder);

    pcmRatioSlider.setup(GuiSlider::Config{ .parent = *this, .id = code + AdpcmPrKey::pcmRatio, .title = AdpcmGuiText::Adpcm::pcmRatio, .isReset = true });
    pcmRatioSlider.setWantsKeyboardFocus(true);
    pcmRatioSlider.setExplicitFocusOrder(++tabOrder);

    // パンポット設定
    panCat.setupHwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visiblePan, .invisibleTitle = AdpcmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::pan, .title = AdpcmGuiText::Adpcm::pan, .isReset = true });
    panSlider.setRange(0.0f, 1.0f);
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    setupPanBtn(panToLBtn, AdpcmGuiText::Adpcm::Pan::l, tabOrder);
    setupPanBtn(panToCBtn, AdpcmGuiText::Adpcm::Pan::c, tabOrder);
    setupPanBtn(panToRBtn, AdpcmGuiText::Adpcm::Pan::r, tabOrder);

    adsrCat.setupSwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visibleAdsr, .invisibleTitle = AdpcmGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    adsrBypassButton.setup({ .parent = *this, .id = code + AdpcmPrKey::adsr + AdpcmPrKey::bypass, .title = AdpcmGuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::stl, .title = AdpcmGuiText::Adpcm::Adsr::stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::ar, .title = AdpcmGuiText::Adpcm::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::dr, .title = AdpcmGuiText::Adpcm::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::sl, .title = AdpcmGuiText::Adpcm::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Adsr::rr, .title = AdpcmGuiText::Adpcm::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setupSwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visiblePitchAdsr, .invisibleTitle = AdpcmGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + AdpcmPrKey::pitchAdsr + AdpcmPrKey::bypass, .title = AdpcmGuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::ar, .title = AdpcmGuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::dr , .title = AdpcmGuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::rr, .title = AdpcmGuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::stl, .title = AdpcmGuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::atl, .title = AdpcmGuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::ssl, .title = AdpcmGuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::PitchAdsr::rll, .title = AdpcmGuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwEnvCat.setupSwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visibleSwEnv, .invisibleTitle = AdpcmGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

    ssgSwEnvBypassButton.setup({ .parent = *this, .id = code + AdpcmPrKey::ssgSwEnv + AdpcmPrKey::bypass, .title = AdpcmGuiText::SsgSwEnv::bypass, .isReset = true });
    ssgSwEnvBypassButton.setWantsKeyboardFocus(true);
    ssgSwEnvBypassButton.setExplicitFocusOrder(++tabOrder);

    ssgSwStepsSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::steps, .title = AdpcmGuiText::SsgSwEnv::steps, .isReset = true });
    ssgSwStepsSlider.setWantsKeyboardFocus(true);
    ssgSwStepsSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwStepsSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwEnvLoopButton.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::loop, .title = AdpcmGuiText::SsgSwEnv::loop, .isReset = true });
    ssgSwEnvLoopButton.setWantsKeyboardFocus(true);
    ssgSwEnvLoopButton.setExplicitFocusOrder(++tabOrder);
    ssgSwEnvLoopButton.onClick = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        ssgSwLoopToSlider.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopToSlider.label.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCountSlider.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCountSlider.label.setEnabled(ssgEnvLoopEnable);

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwLoopToSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::loopTo, .title = AdpcmGuiText::SsgSwEnv::loopTo, .isReset = true });
    ssgSwLoopToSlider.setWantsKeyboardFocus(true);
    ssgSwLoopToSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwLoopToSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwLoopCountSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::loopCount, .title = AdpcmGuiText::SsgSwEnv::loopCount, .isReset = true });
    ssgSwLoopCountSlider.setWantsKeyboardFocus(true);
    ssgSwLoopCountSlider.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

    ssgSwLoopToSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopToSlider.label.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.label.setEnabled(ssgEnvLoopEnable);

    ssgSwStartLevelSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::stl, .title = AdpcmGuiText::SsgSwEnv::stl, .isReset = true });
    ssgSwStartLevelSlider.setWantsKeyboardFocus(true);
    ssgSwStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwR1Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::r1, .title = AdpcmGuiText::SsgSwEnv::r1, .isReset = true });
    ssgSwR1Slider.setWantsKeyboardFocus(true);
    ssgSwR1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL1Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::l1, .title = AdpcmGuiText::SsgSwEnv::l1, .isReset = true });
    ssgSwL1Slider.setWantsKeyboardFocus(true);
    ssgSwL1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR2Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::r2, .title = AdpcmGuiText::SsgSwEnv::r2, .isReset = true });
    ssgSwR2Slider.setWantsKeyboardFocus(true);
    ssgSwR2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL2Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::l2, .title = AdpcmGuiText::SsgSwEnv::l2, .isReset = true });
    ssgSwL2Slider.setWantsKeyboardFocus(true);
    ssgSwL2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR3Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::r3, .title = AdpcmGuiText::SsgSwEnv::r3, .isReset = true });
    ssgSwR3Slider.setWantsKeyboardFocus(true);
    ssgSwR3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL3Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::l3, .title = AdpcmGuiText::SsgSwEnv::l3, .isReset = true });
    ssgSwL3Slider.setWantsKeyboardFocus(true);
    ssgSwL3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR4Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::r4, .title = AdpcmGuiText::SsgSwEnv::r4, .isReset = true });
    ssgSwR4Slider.setWantsKeyboardFocus(true);
    ssgSwR4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL4Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::l4, .title = AdpcmGuiText::SsgSwEnv::l4, .isReset = true });
    ssgSwL4Slider.setWantsKeyboardFocus(true);
    ssgSwL4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR5Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::r5, .title = AdpcmGuiText::SsgSwEnv::r5, .isReset = true });
    ssgSwR5Slider.setWantsKeyboardFocus(true);
    ssgSwR5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL5Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::l5, .title = AdpcmGuiText::SsgSwEnv::l5, .isReset = true });
    ssgSwL5Slider.setWantsKeyboardFocus(true);
    ssgSwL5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR6Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::r6, .title = AdpcmGuiText::SsgSwEnv::r6, .isReset = true });
    ssgSwR6Slider.setWantsKeyboardFocus(true);
    ssgSwR6Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL6Slider.setup({ .parent = *this, .id = code + AdpcmPrKey::SsgSwEnv::l6, .title = AdpcmGuiText::SsgSwEnv::l6, .isReset = true });
    ssgSwL6Slider.setWantsKeyboardFocus(true);
    ssgSwL6Slider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setupSwCategory({ .parent = *this, .title = AdpcmGuiText::Category::visibleDetune, .invisibleTitle = AdpcmGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });

    mul.setup({ .parent = *this, .id = code + AdpcmPrKey::mul, .title = AdpcmGuiText::Detune::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    mulRatio.setup({ .parent = *this, .id = code + AdpcmPrKey::mulRatio, .title = AdpcmGuiText::Detune::MulRatio, .isReset = true });
    mulRatio.setWantsKeyboardFocus(true);
    mulRatio.setExplicitFocusOrder(++tabOrder);

    dt1.setup({ .parent = *this, .id = code + AdpcmPrKey::dt, .title = AdpcmGuiText::Detune::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + AdpcmPrKey::dt2, .title = AdpcmGuiText::Detune::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    unisonCat.setupOtherCategory({ .parent = *this, .title = AdpcmGuiText::Category::invisibleUnison, .invisibleTitle = AdpcmGuiText::Category::invisibleUnison, .enableChangeDetailVisible = true });

    unisonVoicesSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Unison::voices, .title = AdpcmGuiText::Unison::voices, .isReset = true });
    unisonVoicesSlider.setWantsKeyboardFocus(true);
    unisonVoicesSlider.setExplicitFocusOrder(++tabOrder);

    unisonDetuneSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Unison::detune, .title = AdpcmGuiText::Unison::detune, .isReset = true });
    unisonDetuneSlider.setWantsKeyboardFocus(true);
    unisonDetuneSlider.setExplicitFocusOrder(++tabOrder);

    unisonSpreadSlider.setup({ .parent = *this, .id = code + AdpcmPrKey::Unison::spread, .title = AdpcmGuiText::Unison::spread, .isReset = true });
    unisonSpreadSlider.setWantsKeyboardFocus(true);
    unisonSpreadSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = AdpcmGuiText::Category::visibleMvol, .invisibleTitle = AdpcmGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = AdpcmPrKey::masterVol, .title = AdpcmGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    // 音声ファイル読み込みボタン
    loadButton.setup({ .parent = *this, .title = AdpcmGuiText::File::load , .isReset = false });
    loadButton.addListener(&ctx.editor);
    loopButton.setWantsKeyboardFocus(true);
    loopButton.setExplicitFocusOrder(++tabOrder);

    // ロードしているファイル名
    fileNameLabel.setup({ .parent = *this, .title = Io::empty });
    fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    clearButton.setup({ .parent = *this, .title = AdpcmGuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false });
    clearButton.setWantsKeyboardFocus(true);
    clearButton.setExplicitFocusOrder(++tabOrder);
    clearButton.onClick = [this]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            ctx.audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            fileNameLabel.setText(Io::empty, juce::dontSendNotification);
        };

    setupGraph();
    updateGraph();
}

void GuiAdpcm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(AdpcmGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(AdpcmGuiValue::Group::Padding::width, AdpcmGuiValue::Group::Padding::height);
    mRect.removeFromTop(AdpcmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(AdpcmGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // グラフ用の区画を確保
    layoutGraph(mRect);
    updateGraph();

    layoutMainPcm({ .rect = mRect, .loadPcmBtn = &loadButton, .pcmFileNameLabel = &fileNameLabel, .clearPcmBtn = &clearButton });

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutOptionalCat(mRect);

    layoutPanCat(mRect);

    layoutAdsrCat(mRect);

    layoutPitchEnvCat(mRect);

    layoutSsgSwEnvCat(mRect);

    layoutDetuneCat(mRect);

    layoutUnisonCat(mRect);

    layoutQualityCat(mRect);

    layoutMvolCat(mRect);
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
    return button == &panToLBtn;
}

bool GuiAdpcm::isBtnPanC(juce::Button* button)
{
    return button == &panToCBtn;
}

bool GuiAdpcm::isBtnPanR(juce::Button* button)
{
    return button == &panToRBtn;
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

void GuiAdpcm::initParams()
{
    this->ctx.audioProcessor.initParams("ADPCM_");
    this->ctx.audioProcessor.unloadAdpcmFile();
    updateFileName(Io::empty);
}

void GuiAdpcm::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void GuiAdpcm::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiAdpcm::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn, });
    }
}

void GuiAdpcm::layoutAdsrCat(juce::Rectangle<int>& rect)
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

void GuiAdpcm::layoutPitchEnvCat(juce::Rectangle<int>& rect)
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

void GuiAdpcm::layoutDetuneCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &detuneCat });

    bool visible = detuneCat.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &mul.label, .component = &mul });
        layoutMain({ .mainRect = rect, .label = &mulRatio.label, .component = &mulRatio });
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
    }
}

void GuiAdpcm::layoutSsgSwEnvCat(juce::Rectangle<int>& rect)
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

void GuiAdpcm::applySsgSwEnvLoopValues(bool enabled)
{
    if (enabled)
    {
        int steps = static_cast<int>(ssgSwStepsSlider.getValue());

        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (steps < 2) {
            steps = 2;
            ssgSwStepsSlider.setValue(steps);
        }

        int loopTo = static_cast<int>(ssgSwLoopToSlider.getValue());

        // Steps - LoopTo が 2未満のときは、LoopTo を Steps - 2 にする
        if (steps - loopTo < 2) {
            ssgSwLoopToSlider.setValue(steps - 2);
        }
    }
}

void GuiAdpcm::layoutOptionalCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .label = &optionalCat });

    bool visible = optionalCat.isDetailVisible();

    loopButton.setVisible(visible);
    pcmOffsetSlider.setVisibleWithLabel(visible);
    pcmRatioSlider.setVisibleWithLabel(visible);

    if (visible) {
        layoutMain({ .mainRect = rect, .component = &loopButton });
        layoutMain({ .mainRect = rect, .label = &pcmOffsetSlider.label, .component = &pcmOffsetSlider });
        layoutMain({ .mainRect = rect, .label = &pcmRatioSlider.label, .component = &pcmRatioSlider, });
    }
}

void GuiAdpcm::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    auto repaintGraph = [this]() { updateGraph(); };

    adsrBypassButton.onStateChange = repaintGraph;
    pitchAdsrBypassButton.onStateChange = repaintGraph;
    ssgSwEnvBypassButton.onStateChange = repaintGraph;

    startLevelSlider.onValueChange = repaintGraph;
    attackSlider.onValueChange = repaintGraph;
    decaySlider.onValueChange = repaintGraph;
    sustainSlider.onValueChange = repaintGraph;
    releaseSlider.onValueChange = repaintGraph;

    pitchAttackSlider.onValueChange = repaintGraph;
    pitchDecaySlider.onValueChange = repaintGraph;
    pitchReleaseSlider.onValueChange = repaintGraph;
    pitchStartLevelSlider.onValueChange = repaintGraph;
    pitchAttackLevelSlider.onValueChange = repaintGraph;
    pitchSustainLevelSlider.onValueChange = repaintGraph;
    pitchReleaseLevelSlider.onValueChange = repaintGraph;

    ssgSwEnvLoopButton.onStateChange = repaintGraph;

    ssgSwStepsSlider.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();
        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopToSlider.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();
        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopCountSlider.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();
        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };

    ssgSwR1Slider.onValueChange = repaintGraph;
    ssgSwR2Slider.onValueChange = repaintGraph;
    ssgSwR3Slider.onValueChange = repaintGraph;
    ssgSwR4Slider.onValueChange = repaintGraph;
    ssgSwR5Slider.onValueChange = repaintGraph;
    ssgSwR6Slider.onValueChange = repaintGraph;

    ssgSwStartLevelSlider.onValueChange = repaintGraph;
    ssgSwL1Slider.onValueChange = repaintGraph;
    ssgSwL2Slider.onValueChange = repaintGraph;
    ssgSwL3Slider.onValueChange = repaintGraph;
    ssgSwL4Slider.onValueChange = repaintGraph;
    ssgSwL5Slider.onValueChange = repaintGraph;
    ssgSwL6Slider.onValueChange = repaintGraph;

    addAndMakeVisible(graphSeparator);
    graphSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiAdpcm::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiAdpcm::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(AdpcmGuiValue::MainGroup::Graph::height + AdpcmGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(AdpcmGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(AdpcmGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiAdpcm::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        graph.updateBypass(pitchAdsrBypassButton.getToggleState());

        graph.updatePitchEnv(
            pitchAttackSlider,
            pitchDecaySlider,
            pitchReleaseSlider,
            pitchStartLevelSlider,
            pitchAttackLevelSlider,
            pitchSustainLevelSlider,
            pitchReleaseLevelSlider,
            p_curveCore,
            isCurveMode,
            0
        );
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        graph.updateBypass(ssgSwEnvBypassButton.getToggleState());

        graph.updateSsgSwEnv(
            ssgSwStepsSlider,
            ssgSwEnvLoopButton,
            ssgSwLoopToSlider,
            ssgSwLoopCountSlider,
            { nullptr, &ssgSwR1Slider, &ssgSwR2Slider, &ssgSwR3Slider, &ssgSwR4Slider, &ssgSwR5Slider, &ssgSwR6Slider },
            { &ssgSwStartLevelSlider, &ssgSwL1Slider, &ssgSwL2Slider, &ssgSwL3Slider, &ssgSwL4Slider, &ssgSwL5Slider, &ssgSwL6Slider },
            p_curveCore,
            isCurveMode,
            0
        );
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        graph.updateBypass(adsrBypassButton.getToggleState());

        graph.updateAmpEnv(
            startLevelSlider,
            attackSlider,
            decaySlider,
            sustainSlider,
            releaseSlider,
            p_curveCore,
            isCurveMode,
            0
        );
    }
}

void GuiAdpcm::layoutUnisonCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &unisonCat });

    bool visibleUnison = unisonCat.isDetailVisible();

    unisonVoicesSlider.setVisibleWithLabel(visibleUnison);
    unisonDetuneSlider.setVisibleWithLabel(visibleUnison);
    unisonSpreadSlider.setVisibleWithLabel(visibleUnison);

    if (visibleUnison)
    {
        layoutMain({ .mainRect = rect, .label = &unisonVoicesSlider.label, .component = &unisonVoicesSlider });
        layoutMain({ .mainRect = rect, .label = &unisonDetuneSlider.label, .component = &unisonDetuneSlider });
        layoutMain({ .mainRect = rect, .label = &unisonSpreadSlider.label, .component = &unisonSpreadSlider });
    }
}
