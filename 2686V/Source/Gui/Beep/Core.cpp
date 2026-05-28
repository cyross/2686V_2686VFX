#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Beep/Keys.h"
#include "../../Processor/Beep/Values.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"

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
void GuiBeep::setup() {
    juce::String code = BeepPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, BeepGuiText::Group::mainGroup); // GuiText 等に置換

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    volSlider.setup({ .parent = *this, .id = code + BeepPrKey::level, .title = BeepGuiText::Beep::Level, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    catFix.setupSwCategory({ .parent = *this, .title = BeepGuiText::Category::visibleFix, .invisibleTitle = BeepGuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

    fixToggle.setup({ .parent = *this, .id = code + BeepPrKey::fix, .title = BeepGuiText::Beep::Fix, .isReset = true });
    fixToggle.setWantsKeyboardFocus(true);
    fixToggle.setExplicitFocusOrder(++tabOrder);

    freqSlider.setup({ .parent = *this, .id = code + BeepPrKey::fixFreq, .title = BeepGuiText::Beep::FFreq, .isReset = true });
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    freqSlider.setWantsKeyboardFocus(true);
    freqSlider.setExplicitFocusOrder(++tabOrder);

    freqTo2kBtn.setup({ .parent = *this, .title = BeepGuiText::Beep::To2k, .isReset = false, .isResized = false });
    freqTo2kBtn.setWantsKeyboardFocus(true);
    freqTo2kBtn.setExplicitFocusOrder(++tabOrder);

    freqTo2kBtn.onClick = [this] { freqSlider.setValue(2000.0, juce::sendNotification); };
    freqTo2kBtn.setWantsKeyboardFocus(true);
    freqTo2kBtn.setExplicitFocusOrder(++tabOrder);

    adsrCat.setupSwCategory({ .parent = *this, .title = BeepGuiText::Category::visibleAdsr, .invisibleTitle = BeepGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    bypassToggle.setup({ .parent = *this, .id = code + BeepPrKey::adsr + BeepPrKey::bypass, .title = BeepGuiText::Beep::Adsr::Bypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::stl, .title = BeepGuiText::Beep::Adsr::Stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::ar, .title = BeepGuiText::Beep::Adsr::Ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::dr, .title = BeepGuiText::Beep::Adsr::Dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::sl, .title = BeepGuiText::Beep::Adsr::Sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::rr, .title = BeepGuiText::Beep::Adsr::Rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setupSwCategory({ .parent = *this, .title = BeepGuiText::Category::visiblePitchAdsr, .invisibleTitle = BeepGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + BeepPrKey::pitchAdsr + BeepPrKey::bypass, .title = BeepGuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::ar, .title = BeepGuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::dr , .title = BeepGuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::rr, .title = BeepGuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::stl, .title = BeepGuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::atl, .title = BeepGuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::ssl, .title = BeepGuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::rll, .title = BeepGuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwEnvCat.setupSwCategory({ .parent = *this, .title = BeepGuiText::Category::visibleSwEnv, .invisibleTitle = BeepGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

    ssgSwEnvBypassButton.setup({ .parent = *this, .id = code + BeepPrKey::ssgSwEnv + BeepPrKey::bypass, .title = BeepGuiText::SsgSwEnv::bypass, .isReset = true });
    ssgSwEnvBypassButton.setWantsKeyboardFocus(true);
    ssgSwEnvBypassButton.setExplicitFocusOrder(++tabOrder);

    ssgSwStepsSlider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::steps, .title = BeepGuiText::SsgSwEnv::steps, .isReset = true });
    ssgSwStepsSlider.setWantsKeyboardFocus(true);
    ssgSwStepsSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwStepsSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwEnvLoopButton.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::loop, .title = BeepGuiText::SsgSwEnv::loop, .isReset = true });
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

    ssgSwLoopToSlider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::loopTo, .title = BeepGuiText::SsgSwEnv::loopTo, .isReset = true });
    ssgSwLoopToSlider.setWantsKeyboardFocus(true);
    ssgSwLoopToSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwLoopToSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwLoopCountSlider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::loopCount, .title = BeepGuiText::SsgSwEnv::loopCount, .isReset = true });
    ssgSwLoopCountSlider.setWantsKeyboardFocus(true);
    ssgSwLoopCountSlider.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

    ssgSwLoopToSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopToSlider.label.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.label.setEnabled(ssgEnvLoopEnable);

    ssgSwStartLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::stl, .title = BeepGuiText::SsgSwEnv::stl, .isReset = true });
    ssgSwStartLevelSlider.setWantsKeyboardFocus(true);
    ssgSwStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwR1Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::r1, .title = BeepGuiText::SsgSwEnv::r1, .isReset = true });
    ssgSwR1Slider.setWantsKeyboardFocus(true);
    ssgSwR1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL1Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::l1, .title = BeepGuiText::SsgSwEnv::l1, .isReset = true });
    ssgSwL1Slider.setWantsKeyboardFocus(true);
    ssgSwL1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR2Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::r2, .title = BeepGuiText::SsgSwEnv::r2, .isReset = true });
    ssgSwR2Slider.setWantsKeyboardFocus(true);
    ssgSwR2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL2Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::l2, .title = BeepGuiText::SsgSwEnv::l2, .isReset = true });
    ssgSwL2Slider.setWantsKeyboardFocus(true);
    ssgSwL2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR3Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::r3, .title = BeepGuiText::SsgSwEnv::r3, .isReset = true });
    ssgSwR3Slider.setWantsKeyboardFocus(true);
    ssgSwR3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL3Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::l3, .title = BeepGuiText::SsgSwEnv::l3, .isReset = true });
    ssgSwL3Slider.setWantsKeyboardFocus(true);
    ssgSwL3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR4Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::r4, .title = BeepGuiText::SsgSwEnv::r4, .isReset = true });
    ssgSwR4Slider.setWantsKeyboardFocus(true);
    ssgSwR4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL4Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::l4, .title = BeepGuiText::SsgSwEnv::l4, .isReset = true });
    ssgSwL4Slider.setWantsKeyboardFocus(true);
    ssgSwL4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR5Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::r5, .title = BeepGuiText::SsgSwEnv::r5, .isReset = true });
    ssgSwR5Slider.setWantsKeyboardFocus(true);
    ssgSwR5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL5Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::l5, .title = BeepGuiText::SsgSwEnv::l5, .isReset = true });
    ssgSwL5Slider.setWantsKeyboardFocus(true);
    ssgSwL5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR6Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::r6, .title = BeepGuiText::SsgSwEnv::r6, .isReset = true });
    ssgSwR6Slider.setWantsKeyboardFocus(true);
    ssgSwR6Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL6Slider.setup({ .parent = *this, .id = code + BeepPrKey::SsgSwEnv::l6, .title = BeepGuiText::SsgSwEnv::l6, .isReset = true });
    ssgSwL6Slider.setWantsKeyboardFocus(true);
    ssgSwL6Slider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setupSwCategory({ .parent = *this, .title = BeepGuiText::Category::visibleDetune, .invisibleTitle = BeepGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });

    mul.setup({ .parent = *this, .id = code + BeepPrKey::mul, .title = BeepGuiText::Detune::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    mulRatio.setup({ .parent = *this, .id = code + BeepPrKey::mulRatio, .title = BeepGuiText::Detune::MulRatio, .isReset = true });
    mulRatio.setWantsKeyboardFocus(true);
    mulRatio.setExplicitFocusOrder(++tabOrder);

    dt1.setup({ .parent = *this, .id = code + BeepPrKey::dt, .title = BeepGuiText::Detune::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + BeepPrKey::dt2, .title = BeepGuiText::Detune::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = BeepGuiText::Category::visibleMonoMode, .invisibleTitle = BeepGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = BeepPrKey::monoMode, .title = BeepGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = BeepGuiText::Category::visibleMvol, .invisibleTitle = BeepGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = BeepPrKey::masterVol, .title = BeepGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    setupGraph();
    updateGraph();
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(BeepGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(BeepGuiValue::Group::Padding::width, BeepGuiValue::Group::Padding::height);
    mRect.removeFromTop(BeepGuiValue::Group::TitlePaddingTop);

    // グラフ用の区画を確保
    layoutGraph(mRect);
    updateGraph();

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(BeepGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    layoutMain({ .mainRect = mRect, .label = &volSlider.label, .component = &volSlider });

    layoutFixCat(mRect);

    layoutAdsrCat(mRect);

	layoutPitchEnvCat(mRect);

    layoutSsgSwEnvCat(mRect);

	layoutDetuneCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);
}

void GuiBeep::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiBeep::initParams()
{
    this->ctx.audioProcessor.initParams("BEEP_");
}

void GuiBeep::layoutFixCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &catFix });

    bool visibleFix = catFix.isDetailVisible();

    fixToggle.setVisible(visibleFix);
    freqSlider.setVisibleWithLabel(visibleFix);
    freqTo2kBtn.setVisible(visibleFix);

    if (visibleFix)
    {
        layoutMain({ .mainRect = rect, .component = &fixToggle });
        layoutMain({ .mainRect = rect, .label = &freqSlider.label, .component = &freqSlider });
        layoutMain({ .mainRect = rect, .component = &freqTo2kBtn, });
    }
}

void GuiBeep::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiBeep::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiBeep::layoutAdsrCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &adsrCat });

    bool visible = adsrCat.isDetailVisible();

    bypassToggle.setVisible(visible);
    startLevelSlider.setVisibleWithLabel(visible);
    attackSlider.setVisibleWithLabel(visible);
    decaySlider.setVisibleWithLabel(visible);
    sustainSlider.setVisibleWithLabel(visible);
    releaseSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &bypassToggle });
        layoutMain({ .mainRect = rect, .label = &startLevelSlider.label, .component = &startLevelSlider });
        layoutMain({ .mainRect = rect, .label = &attackSlider.label, .component = &attackSlider });
        layoutMain({ .mainRect = rect, .label = &decaySlider.label, .component = &decaySlider });
        layoutMain({ .mainRect = rect, .label = &sustainSlider.label, .component = &sustainSlider });
        layoutMain({ .mainRect = rect, .label = &releaseSlider.label, .component = &releaseSlider });
    }
}

void GuiBeep::layoutPitchEnvCat(juce::Rectangle<int>& rect)
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

void GuiBeep::layoutDetuneCat(juce::Rectangle<int>& rect)
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

void GuiBeep::layoutSsgSwEnvCat(juce::Rectangle<int>& rect)
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

void GuiBeep::applySsgSwEnvLoopValues(bool enabled)
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

void GuiBeep::setupGraph()
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

void GuiBeep::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiBeep::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(BeepGuiValue::MainGroup::Graph::height + BeepGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(BeepGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(BeepGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiBeep::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
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
